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

package kvminfo

import (
	"unicode"

	goversion "github.com/hashicorp/go-version"
)

type Check struct {
	OS           string `json:"os"`
	Feature      string `json:"feature"`
	KernelMinVer string `json:"kernelminver"`
	KernelMaxVer string `json:"kernelmaxver"`
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
