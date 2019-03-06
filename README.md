# kvm-info-nfd-plugin

`kvm-info-nfd-plugin` is an addon for the [kubernetes node feature discovery](https://github.com/kubernetes-sigs/node-feature-discovery).
It plugs in the [user-specific features support](https://github.com/kubernetes-sigs/node-feature-discovery#local-user-specific-features), and add labels to the hosts
to report if they support [KVM HyperV enlightenments](https://devconfcz2019.sched.com/event/Jcen/enlightening-kvm-hyper-v-emulation).

This is useful for [kubevirt](https://kubevirt.io), which can use these labels to safely and efficiently schedule VMs on hyoerv-capable hosts.

## scope and purpose

Checking KVM support in kernels is hard. In general, things are supposed to just work, but bugs, backports and various issues may result
in some kernel versions not work properly.

This project don't aim to perfectly represent the state of the Hyper-V support. That would be impractical. Instead, this project aim to provide
a corse-grained labelling to whitelist or blacklist some kernel versions.

We believe this is a good compromise between maintenability and user experience.

## hyperv labelling

QEMU exposes [quite a lot of hyperv enlightenments](https://libvirt.org/formatdomain.html#elementsFeatures). We choose to _not_ expose all of them,
to avoid a combinatorial explosion in the check matrix.
Thus, `kvm-info-nfd-plugin` just exports two set of labels:
- hyperv: signals that the host is whitelisted, thus has no known gotchas, thus _any_ enlightenment can be safely turned on
- hyperv-lev2: same as above but for L2 (nested hyperv) enlightenments. These are separated because enabling L2 enlightenments without running nested
  virtualization may cause a small performance loss.

