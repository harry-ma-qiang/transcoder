
import os
import sys
import json
import hashlib
import requests
from jinja2 import Template

def createVoloJobFile(srcInfo, profile):
    with open(srcInfo) as i:
        srcInfo = json.load(i)
        _src = '/tmp/demo/src/' + os.path.basename(srcInfo['url'])
        _out = '/tmp/demo/out/' + profile + '_' + os.path.basename(_src)
        _log = '/tmp/demo/out/'
        _jid = hashlib.md5(_src.encode('utf-8')).hexdigest()
        _tid = _jid + '.' + profile
        profileFile = profile + '.json'
        APIURL = 'http://127.0.0.1:8500/v1/kv/job/' + _jid + '/' + profile
        print(_src, _out, _log, _tid, profileFile, APIURL)

        with open(profileFile) as p:
            tempStr = p.read()
            t = Template(tempStr)
            jsonStr = t.render(SRC=_src, OUT=_out, LOG=_log, JID=_tid)
            print(jsonStr)

            r = requests.put(url = APIURL, data = jsonStr)
            print(r.text)
        # End of prifile 
    # End of with info json
    
# End of createVoloJobFile()

def createNomadJobSpec(srcInfo, profiles):
    with open(srcInfo) as i:
        srcInfo = json.load(i)
        _src = '/tmp/demo/src/' + os.path.basename(srcInfo['url'])
        _jid = hashlib.md5(_src.encode('utf-8')).hexdigest()

        jobSpec = {'Job':{}}
        jobSpec['Job']['ID'] = _jid
        jobSpec['Job']['Type'] = 'batch'
        jobSpec['Job']['Priority'] = 50
        jobSpec['Job']['Datacenters'] = ['dc1']
        jobSpec['Job']['TaskGroups'] = []

        for p in profiles :
            taskGroup = {}
            taskGroup['Name'] = p
            taskGroup['Tasks'] = []

            task = {}
            task['Name'] = p
            task['Driver'] = 'raw_exec'
            task['Config'] = {}
            task['Config']['command'] = '/tmp/demo/bin/volo-2.0-v001'
            task['Config']['args'] = []
            task['Config']['args'].append("--config")

            ConsulAPIURL = 'http://127.0.0.1:8500/v1/kv/job/' + _jid + '/' + p
            task['Config']['args'].append(ConsulAPIURL)

            taskGroup['Tasks'].append(task)
            jobSpec['Job']['TaskGroups'].append(taskGroup)

        # End of for

        jsonStr = json.dumps(jobSpec)
        print(jsonStr)

        NomadAPIURL = 'http://127.0.0.1:4646/v1/jobs'
        r = requests.post(url = NomadAPIURL, data = jsonStr)
        print('Nomad:', r.text)
    # End of with

# End of createNomadJobSpec

if __name__ == "__main__":
    try:
        src = sys.argv[1]
        # print(src)
    except:
        sys.exit(1)

    # createVoloJobFile(srcInfo=src, profile='480p')
    # createVoloJobFile(srcInfo=src, profile='640p')
    createVoloJobFile(srcInfo=src, profile='1080p')
    createNomadJobSpec(srcInfo=src, profiles=['1080p'])

# End of main