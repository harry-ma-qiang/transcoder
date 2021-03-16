#!/bin/bash

export PATH=$PATH:${PWD}/bin:.:/tmp

SRC="${1}"

# Need a better absolute path place in production, e.g. /opt/volo/bin/volo-2.0-v001
[ -d /tmp/demo/ ] || ln -s ${PWD} /tmp/demo

if ! volo-2.0-v001 --info "${SRC}"; then
    echo "Invalid media file"
    exit 1
fi

# Clean finished jobs and previous outputs
curl -X PUT http://127.0.0.1:4646/v1/system/gc
rm -f out/*.*
sleep 1

python3 job_spec.py "${SRC}.info.json"


# These files can be kept for other workflow
# rm "${SRC}.info.json" "${SRC}.thumbnail.png"
# http://localhost:8500/ui/dc1/kv
# http://localhost:4646/ui/jobs

# exit 0 for nomad scheduler
exit 0
