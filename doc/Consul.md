## Consul server setup
Install Docker
```
sudo yum -y install docker
```

Run Docker
```
sudo docker run -d --name=dev-consul --net=host consul:0.7.5 \
consul agent -dev -ui
```

Externally visible
```
sudo docker run -d --name=consul --net=host consul:0.7.5 \
consul agent -ui -server -bind=<external ip> -client=<external ip> -bootstrap-expect=1
```

For more information:
https://hub.docker.com/_/consul/

## GUI
There is currently no generalised Configuration GUI, although this feature is planned.
For now, we recommend using backend-specific GUIs.

Consul has a built-in web UI, so we recommend using that if a GUI is needed.
It can be accessed using the `/ui` URL path, for example `http://myconsulserver:8500/ui`
For more information:
https://www.consul.io/intro/getting-started/ui.html
