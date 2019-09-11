/*
 * api.js: Simple javascript to demonstrate api api.
 * Author: harry.ma@dao-lab.com
 */
document.head.innerHTML += '<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">';
document.head.innerHTML += '<title>api Video Optimizer</title>';
document.head.innerHTML += '<meta charset="utf-8">';
document.head.innerHTML += '<meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0">';
document.head.innerHTML += '<link rel="stylesheet" type="text/css" href="api.css">';
document.head.innerHTML += '<link href="https://vjs.zencdn.net/7.3.0/video-js.css" rel="stylesheet">';
document.head.innerHTML += '<script src="https://cdn.bootcss.com/jquery/3.2.1/jquery.slim.min.js" integrity="sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN" crossorigin="anonymous"></script>';
document.head.innerHTML += '<script src="https://cdn.bootcss.com/popper.js/1.12.9/umd/popper.min.js" integrity="sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q" crossorigin="anonymous"></script>';
document.head.innerHTML += '<script src="https://cdn.bootcss.com/bootstrap/4.0.0/js/bootstrap.min.js" integrity="sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl" crossorigin="anonymous"></script>';
document.body.innerHTML += '<h3>api Cube Job Summary</h3>';
document.body.innerHTML += '<div id="api" class="container"></div>';

var jobs_list = {};

function loadJson(url, callback = null, item = null) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            try {
                var obj = JSON.parse(this.responseText);
                if (obj) {
                    if (typeof(callback) == "function") {
                        callback(obj, item);
                    }
                }
            } catch (e) {}
        }
    };
    xhttp.open("GET", url, true);
    xhttp.send();
}

function updateJson(jobOld, jobNew, filename) {
    var result = {};
    for (var key in jobOld) result[key] = jobOld[key];
    for (var key in jobNew) result[key] = jobNew[key];

    var xhttp = new XMLHttpRequest();
    xhttp.open("PUT", filename, true);
    xhttp.send(JSON.stringify(result));
}

function start_stop_job(jobName, to_change) {
    var jobNew = {
        "enable": to_change ? "1" : "0"
    };
    var job_url = '/api/' + jobName + '.job';
    updateJson(jobs_list[jobName], jobNew, job_url, loadJson, job_url);
    loadJson(job_url, show_job, job_url);
}

function update_output_rtmp(jobName, srv_url_id, srv_key_id, save_output_id) {
    var url_new = document.getElementById(srv_url_id).value.replace(/\/$/, "") + '/' + document.getElementById(srv_key_id).value.replace(/^\//, "").replace(/\/$/, "");
    var url_parts = url_new.split('/');
    if (url_new.startsWith('rtmp://') && url_parts.length == 5) {
        var jobNew = {
            "output": url_new,
            "version": Date.now().toString()
        };
        var job_url = '/api/' + jobName + '.job';
        updateJson(jobs_list[jobName], jobNew, job_url, loadJson, job_url);
        loadJson(job_url, show_job, job_url);
        document.getElementById(save_output_id).value = "save";
    } else {
        alert("Invalid rtmp url. it looks like rtmp://ip:port/app/stream");
    }
}

function load_preview(jobName, input_id, player_id) {
    var isNotEmpty = (document.getElementById(input_id) 
                        && document.getElementById(input_id).value 
                        && document.getElementById(input_id).value.length>0);
    if (jobName == "03_facebook") {
        if (isNotEmpty) {
            document.getElementById(player_id).innerHTML = 
                '<iframe src="https://www.facebook.com/video/embed?video_id=' + document.getElementById(input_id).value + '" style="width:98%;height:100%;height:240px;" scrolling="no" frameborder="0" allowtransparency="true" allow="encrypted-media" allowfullscreen="true"></iframe>';
        } else {
            document.getElementById(player_id).innerHTML = "NO video_id given";
        }
    } else if (jobName == "04_youtube") {
        if (isNotEmpty) {
            document.getElementById(player_id).innerHTML = 
                '<iframe src="https://www.youtube.com/embed/' + document.getElementById(input_id).value + '" style="width:98%;height:100%;height:240px;" scrolling="no" frameborder="0" allowtransparency="true" allow="encrypted-media" allowfullscreen="true"></iframe>';
        } else {
            document.getElementById(player_id).innerHTML = "NO video_id given";
        }
    } else if (jobName == "02_record") {
        if (!document.getElementById('record-video')) {
            document.getElementById(player_id).innerHTML = 
                '<video id="record-video" class="video-js vjs-default-skin" controls preload="auto" style="width:98%;height:100%;height:240px;" ><source src="index.m3u8" type="application/x-mpegURL"></video>'
            videojs('record-video');
        }
    }
}

function render_label(jobObj, jobName, itemObj, itemName, itemValue) {
    if (itemObj.id.split(".")[1] == "name") {
        itemObj.style = "float:left;border:none;";
    }
    itemObj.innerHTML = itemName + ": " + itemValue;
}

function render_enable(jobObj, jobName, itemObj, itemName, itemValue) {
    if (itemName == "enable") {
        var label = "Start";
        var to_change = "1";
        if (itemValue == 1 || itemValue == "true") {
            label = "Stop";
            var to_change = "0";
        }
        itemObj.innerHTML = '<input type="button"  onclick="start_stop_job(\'' + jobName + '\', ' + to_change + ')" value=' + label + '>';
        itemObj.style = "height:50px;padding-top:10px;border:none;margin-bottom:-10px;padding-left:150px;";
    }
}

function render_output(jobObj, jobName, itemObj, itemName, itemValue) {
    if (itemName == "output") {
        var url_parts = itemValue.split('/');
        if (itemValue.startsWith('rtmp://') && url_parts.length == 5) {
            if (itemObj.innerHTML == "") {
                var server_url_id = jobName + '.srv.url';
                var server_key_id = jobName + '.srv.key';
                var save_output_id = jobName + '.save.output';

                var server_url = 'url: <input id="' + server_url_id + '" type="text" style=\"margin-left:7px;\" value="rtmp://' + url_parts[2] + '/' + url_parts[3] + '" oninput="document.getElementById(\'' + save_output_id + '\').value=\'*save\'">';
                var server_key = 'key: <input id="' + server_key_id + '" type="text" value="' + url_parts[4] + '" oninput="document.getElementById(\'' + save_output_id + '\').value=\'*save\'">';
                var button = '<input id="' + save_output_id + '" type="button" value="save" onclick="update_output_rtmp(\'' + jobName + '\', \'' + server_url_id + '\', \'' + server_key_id + '\', \'' + save_output_id + '\')">';
                itemObj.innerHTML += server_url + "<br/>" + server_key + "<br/>" + button;
            }
        } else {
            render_label(jobObj, jobName, itemObj, itemName, itemValue)
        }
    }
}

function render_preview(jobObj, jobName, itemObj, itemName, itemValue) {
    if (jobName != "01_capture" && itemObj.innerHTML == "") {
        var player_id = jobName + '.preview.player';
        var input_id = jobName + '.preview.input';
        itemObj.innerHTML = 'preview: <input type="text" id="' + input_id + '"> <input type="button" value="load" onclick="load_preview(\'' + jobName + '\', \'' + input_id + '\', \'' + player_id + '\')"><div style="border:none;" class="video_div" id="' + player_id + '"></div>';
        if (jobName == "record") {
            itemObj.innerHTML = 'preview: index.m3u8 <input type="button" value="load" onclick="load_preview(\'' + jobName + '\', \'' + input_id + '\', \'' + player_id + '\')"><div style="border:none;" class="video_div" id="' + player_id + '"></div>';
        }
        document.getElementById(input_id).style.width = "50%";
    }
}

function set_item(jobObj, jobName, itemName, fn_render) {
    if (jobObj.hasOwnProperty(itemName)) {
        fn_render(jobObj, jobName, document.getElementById(jobName + '.' + itemName), itemName, jobObj[itemName])
    }
}

function show_job(jobObj, url) {
    jobName = url.substring(0, url.lastIndexOf('.')) || url

    var item_list = ["name", "enable", "state", "ifps", "ibitrate", "speed", "out_time", "output", "preview"];
    for (var i = 0; i < item_list.length; i++) {
        if (!document.getElementById(jobName + '.' + item_list[i])) {
            document.getElementById(jobName).innerHTML += '<div class="field_div" id="' + jobName + '.' + item_list[i] + '">';
        }
    }

    set_item(jobObj, jobName, "name", render_label);
    if (jobObj.hasOwnProperty("report")) {
        set_item(jobObj.report, jobName, "ifps", render_label);
        set_item(jobObj.report, jobName, "speed", render_label);
        set_item(jobObj.report, jobName, "ibitrate", render_label);
        set_item(jobObj.report, jobName, "out_time", render_label);
    }
    set_item(jobObj, jobName, "enable", render_enable);
    set_item(jobObj, jobName, "output", render_output);
    set_item(jobObj, jobName, "state", render_label);

    if (url.split('.').pop() == 'job') {
        jobs_list[jobName] = jobObj;
    }

    set_item({
        "preview": 1
    }, jobName, "preview", render_preview);
}

function list_job(jobObj) {
    jobObj = jobObj.sort();
    for (var i = 0; i < jobObj.length; i++) {
        if (jobObj[i].file && (jobObj[i].file.split('.').pop() == "job" || jobObj[i].file.split('.').pop() == "state")) {
            var jobName = jobObj[i].file.substring(0, jobObj[i].file.lastIndexOf('.')) || jobObj[i].file
            if (!document.getElementById(jobName)) {
                document.getElementById("api").innerHTML += '<div class="job_div col-lg-3 col-md-6 col-sm-12 col-xs-12" id="' + jobName + '">';
            }
            loadJson('/api/' + jobObj[i].file, show_job, jobObj[i].file);
        }

    }
}

function update() {
    loadJson('/api/', list_job);
}

update();
setInterval(function() {
    update();
}, 3000);