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
 * Copyright 2019 Red Hat, Inc.
 */

package main

import (
	"fmt"
	"os"

	"github.com/kubevirt/kvm-info-nfd-plugin/pkg/kvminfo/versioninfo"
)

const (
	ConfigFilePath string = "/etc/kubernetes/node-feature-discovery/source.d/conf/kvm-version-info.json"
)

func parseArgs() string {
	if len(os.Args) == 2 {
		arg := os.Args[1]
		if arg == "-h" || arg == "--help" || arg == "help" {
			fmt.Fprintf(os.Stderr, "usage: %s [config_file_path]\n", os.Args[0])
			fmt.Fprintf(os.Stderr, "default config_file_path is '%s'\n", ConfigFilePath)
			os.Exit(0)
		}
		return arg
	}
	return ConfigFilePath
}

func main() {
	confFilePath := parseArgs()

	features, err := versioninfo.Run(confFilePath, os.Stderr)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error checking kernel version: %v\n", err)
		os.Exit(1)
	}

	for _, feature := range features {
		fmt.Printf("/kvm-info-version-%s\n", feature)
	}
}
