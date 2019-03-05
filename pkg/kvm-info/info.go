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
	"fmt"
	"io"
)

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
