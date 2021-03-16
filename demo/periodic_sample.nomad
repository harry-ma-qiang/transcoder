job "watch_folder" {
    datacenters = ["dc1"]
    type = "batch"

    periodic {
        cron = "*/5 * * * * * *"
        prohibit_overlap = true
    }

    group "periodic" {
        restart {
            attempts = 3
            delay    = "10s"
        }
        task "periodic" {
            driver = "raw_exec"
            config {
                command = "/Users/harry/Downloads/transcoder-master/demo/example_ls.bash"
            }
            resources {
                memory = 100
            }
            logs {
                max_files     = 1
                max_file_size = 1
            }
        }
    }
}