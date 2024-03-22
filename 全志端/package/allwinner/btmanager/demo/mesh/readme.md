test example:

1. device for provisioner:

   bt_mesh_test
   (1) enable                       :  enable 1 0 11111111111111111111111111000000
   (2) scan                         :  unprov_scan 1 10
   (3) provsioning                  :  prov_node 11111111111111111111111111666666 0
   (4) add net key for local device :  netkey-add 0 1
   (5) add net key for remote device:  netkey-add 2 1
   (6) add app key for local  device:  appkey-add 0 1 0
   (7) add app key for remote node  :  appkey-add 2 1 0
   (8) bind configure server model for local device   :  bind 0 0 1000 0
   (9) bind configure client model for local device   :  bind 0 0 1001 0
   (10) bind configure client model for remote device :  bind 2 0 1000 0
   (11) bind configure client model for remote device :  bind 2 0 1001 0
   (12) generic goo  on             :  goo_client 0 2 0 0
   (13) generic goo off             :  goo_client 0 2 0 1


2. device for node:

   bt_mesh_test
   (1) enable : enable 0 0 11111111111111111111111111666666
