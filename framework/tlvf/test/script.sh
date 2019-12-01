docker container run -e USER=cor -e INSTALL_DIR=/home/cor/work/dev1/build/install --privileged --network prplMesh-net --expose 5000 -v /home/cor/work/dev1/build/install:/home/cor/work/dev1/build/install -v /home/cor/work/dev1/prplMesh:/home/cor/work/dev1/prplMesh --name gateway -d prplmesh-runner 172.19.0.30/16 00:11:22:33 start-controller-agent
sleep 3

docker container run -e USER=cor -e INSTALL_DIR=/home/cor/work/dev1/build/install --privileged --network prplMesh-net --expose 5000 -v /home/cor/work/dev1/build/install:/home/cor/work/dev1/build/install -v /home/cor/work/dev1/prplMesh:/home/cor/work/dev1/prplMesh --name repeater1 -d prplmesh-runner 172.19.0.184/16 aa:bb:cc:00 start-agent

sleep 3

docker container run -e USER=cor -e INSTALL_DIR=/home/cor/work/dev1/build/install --privileged --network prplMesh-net --expose 5000 -v /home/cor/work/dev1/build/install:/home/cor/work/dev1/build/install -v /home/cor/work/dev1/prplMesh:/home/cor/work/dev1/prplMesh --name repeater2 -d prplmesh-runner 172.19.0.176/16 aa:bb:cc:11 start-agent
sleep 3
