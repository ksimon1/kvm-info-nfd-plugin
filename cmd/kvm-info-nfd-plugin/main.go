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

	"github.com/fromanirh/kvm-info-nfd-plugin/pkg/kvminfo/kvmcapabilities"
	"github.com/fromanirh/kvm-info-nfd-plugin/pkg/kvminfo/versioninfo"
)

type Info uint32

const (
	KVM_INFO_VERSION Info = 1 << iota
	KVM_INFO_CAPABILITIES

	KVM_INFO_ALL = KVM_INFO_VERSION | KVM_INFO_CAPABILITIES
)

const (
	ConfigFilePath string = "/etc/kubernetes/node-feature-discovery/source.d/conf/kvm-version-info.json"
)

func parseArgs() (string, Info) {
	info := KVM_INFO_ALL
	if os.Args[0] == "kvm-version-info-nfd-plugin" {
		info = KVM_INFO_VERSION
	} else if os.Args[0] == "kvm-caps-info-nfd-plugin" {
		info = KVM_INFO_CAPABILITIES
	}

	if len(os.Args) == 2 {
		arg := os.Args[1]
		if arg == "-h" || arg == "--help" || arg == "help" {
			fmt.Fprintf(os.Stderr, "usage: %s [config_file_path]\n", os.Args[0])
			fmt.Fprintf(os.Stderr, "default config_file_path is '%s'\n", ConfigFilePath)
			os.Exit(0)
		}
		return arg, info
	}
	return ConfigFilePath, info
}

func main() {
	confFilePath, info := parseArgs()

	if (info & KVM_INFO_VERSION) != 0 {
		features, err := versioninfo.Run(confFilePath, os.Stderr)
		if err != nil {
			fmt.Fprintf(os.Stderr, "error checking kernel version: %v\n")
			os.Exit(1)
		}

		for _, feature := range features {
			fmt.Printf("/kvm-info-version-%s\n", feature)
		}
	}

	if (info & KVM_INFO_CAPABILITIES) != 0 {
		caps, err := kvmcapabilities.Get()
		if err != nil {
			fmt.Fprintf(os.Stderr, "error checking kernel capabilities: %v\n")
			os.Exit(1)
		}
		for _, kvmcap := range caps {
			fmt.Printf("/kvm-info-capability-%s\n", kvmcap)
		}
	}
}
