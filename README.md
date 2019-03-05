# kvm-info-nfd-plugin

`kvm-info-nfd-plugin` is an addon for the [kubernetes node feature discovery](https://github.com/kubernetes-sigs/node-feature-discovery).
It plugs in the [user-specific features support](https://github.com/kubernetes-sigs/node-feature-discovery#local-user-specific-features), and add labels to the hosts
to report if they support [KVM HyperV enlightenments](https://devconfcz2019.sched.com/event/Jcen/enlightening-kvm-hyper-v-emulation).

This is useful for [kubevirt](https://kubevirt.io), which can use these labels to safely and efficiently schedule VMs on hyoerv-capable hosts.

## hyperv labelling

TODO
