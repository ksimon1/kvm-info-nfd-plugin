/*
 * This file is part of the KubeVirt project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright 2018 Red Hat, Inc.
 */

package kvmmain

import (
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"path"
	"strings"
	"unicode"

	"github.com/cobaugh/osrelease"
	goversion "github.com/hashicorp/go-version"
)

type Check struct {
	OS           string `json:"os"`
	Feature      string `json:"feature"`
	KernelMinVer string `json:"kernelminver"`
	KernelMaxVer string `json:"kernelmaxver"`
}

type Config struct {
	OSRelease string  `json:"osrelease"`
	ProcDir   string  `json:"procdir"`
	Checks    []Check `json:"checks"`
}

func Features() []string {
	return []string{
		"hyperv",
	}
}

func (c *Check) IsValid(features []string) bool {
	if c.Feature == "" || c.OS == "" {
		return false
	}
	if c.KernelMinVer == "" && c.KernelMaxVer == "" {
		return false
	}

	for _, feature := range features {
		if feature == c.Feature {
			return true
		}
	}
	return false
}

func (c *Check) IsSatisfied(os string, ver *goversion.Version) (bool, error) {
	if c.OS != os {
		return false, nil
	}
	if c.KernelMinVer != "" {
		minVer, err := goversion.NewVersion(sanitizeVersion(c.KernelMinVer))
		if err != nil {
			return false, err
		}
		if ver.Compare(minVer) < 0 {
			return false, nil
		}
	}
	if c.KernelMaxVer != "" {
		maxVer, err := goversion.NewVersion(sanitizeVersion(c.KernelMaxVer))
		if err != nil {
			return false, err
		}
		if ver.Compare(maxVer) > 0 {
			return false, nil
		}
	}
	return true, nil
}

func OSRelease(conf *Config) (map[string]string, error) {
	if conf.OSRelease != "" {
		return osrelease.ReadFile(conf.OSRelease)
	}
	return osrelease.Read()
}

func KernelVersion(conf *Config) (*goversion.Version, error) {
	procVersion := "/proc/version"
	if conf.ProcDir != "" {
		procVersion = path.Join(path.Clean(conf.ProcDir), "version")
	}
	versionData, err := ioutil.ReadFile(procVersion)
	if err != nil {
		return nil, err
	}
	items := strings.Split(string(versionData), " ")
	// items[0] = "Linux"
	// items[1] = "version"
	return goversion.NewVersion(sanitizeVersion(items[2]))
}

func ReadConfig(confPath string) (*Config, error) {
	fh, err := os.Open(confPath)
	if err != nil {
		return nil, err
	}
	defer fh.Close()
	conf := Config{}
	dec := json.NewDecoder(fh)
	err = dec.Decode(&conf)
	if err != nil {
		return nil, err
	}
	return &conf, nil
}

func Run(confPath string, w io.Writer) ([]string, error) {
	detected := []string{}
	conf, err := ReadConfig(confPath)
	if err != nil {
		return detected, err
	}

	osrelease, err := OSRelease(conf)
	if err != nil {
		return detected, err
	}

	kernelver, err := KernelVersion(conf)
	if err != nil {
		return detected, err
	}

	os := osrelease["ID"]
	features := Features()
	for _, check := range conf.Checks {
		if !check.IsValid(features) {
			fmt.Fprintf(w, "INVALID check: %#v, skipped\n", check)
			continue
		}
		ok, err := check.IsSatisfied(os, kernelver)
		if err != nil {
			fmt.Fprintf(w, "FAILED check: %#v (os=%v kernel=%v) %v\n", check, os, kernelver, err)
			continue
		}
		if !ok {
			fmt.Fprintf(w, "UNFULFILLED check: %#v (os=%v kernel=%v)\n", check, os, kernelver)
			continue
		}
		detected = append(detected, check.Feature)
	}

	return detected, nil
}

func sanitizeVersion(ver string) string {
	var pc rune = 0x00
	for i, c := range ver {
		if unicode.IsDigit(c) || c == '-' || c == '.' {
			pc = c
			continue
		}
		if i > 0 && !unicode.IsDigit(pc) {
			return ver[:i-1]
		}
		return ver[:i]
	}
	return ver
}
