# 🔧 Chat Room – Docker Helper Scripts

This project uses **Docker Compose** to build and run a complete development environment that includes:

- **chat_room_env** → interactive build & run container  
- **postgres_db** → PostgreSQL database  
- **pgadmin** → pgAdmin web UI for managing the database  

To make everything simple and consistent, three helper scripts are provided in the `scripts/` directory:

- `scripts/build_docker_images` — builds all Docker images (chat room environment, PostgreSQL, pgAdmin)
- `scripts/start_chat_room_env` — starts PostgreSQL + pgAdmin in the background, then launches the `chat_room_env` container interactively  
- `scripts/stop_chat_room_env` — stops and removes all containers defined in `docker-compose.yml`

No `sudo` is required (your user must be in the `docker` group).

---

## 🚀 Prerequisites

- Docker and Docker Compose installed  
- Docker daemon running  
- Run scripts from the project root (the same directory where `docker-compose.yml` is located)

---

## 📦 Build All Docker Images

This builds all images used by Docker Compose:

```bash
./scripts/build_docker_images
```

### Images Included

- **chat_room_env** – build + run environment  
- **postgres:15** – official PostgreSQL image  
- **pgadmin** – official pgAdmin4 image  

---

## ▶️ Start the Full Environment

This will start:

- **postgres_db** – running in the background  
- **pgadmin** – running in the background  
- **chat_room_env** – interactive shell for development  

Run:

```bash
./scripts/start_chat_room_env
```

## ⏹ Stop the Environment

Stop and remove all running containers:

```bash
./scripts/stop_chat_room_env
```

PostgreSQL data is not lost, because it is stored inside a persistent Docker volume named pgdata.

## 🌐 Accessing pgAdmin

Once the containers are running, open pgAdmin in your browser:

```bash
http://localhost:8080
```

Login Credentials
- Email: admin
- Password: admin
