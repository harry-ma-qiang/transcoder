# Import the framework
from flask import Flask, jsonify, request
from flask_classful import FlaskView, route
from flask_expects_json import expects_json
from DbManagers.db_manager import DBManager

add_task_schema = {
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
        "taskId": {"type": "string"},
        "videoSource": {"type": "string"},
        "config": {
            "type": "array",
            "minItems": 1,
            "maxItems": 1,
            "items":
            {
                "type": "object",
                "properties": {
                    "video": {
                        "type": "object",
                        "properties": {
                            "codec": {"type": "string"},
                            "bitRate": {"type": "number"},
                            "rateControlMethod": {"type": "string"},
                            "profile": {"type": "string"},
                            "frameRate": {"type": "number"},
                            "resolution": {
                                "type": "object",
                                "properties": {
                                    "width": {"type": "number"},
                                    "height": {"type": "number"}
                                },
                                "additionalProperties": False,
                                "required": ["width", "height"]
                            }
                        },
                        "additionalProperties": False,
                        "required": ["codec", "bitRate", "rateControlMethod", "profile", "frameRate", "resolution"]
                    },
                    "audio": {
                        "type": "object",
                        "properties": {
                            "codec": {"type": "string"},
                            "sampleRate": {"type": "number"},
                            "bitRate": {"type": "number"}
                        },
                        "additionalProperties": False,
                        "required": ["codec", "sampleRate", "bitRate"]
                    }
                },
                "additionalProperties": False,
                "required": ["video", "audio"]
            }
        }
    },
    "additionalProperties": False,
    "required": ["taskId", "videoSource", "config"]
}


class TaskView(FlaskView):
    def __init__(self, db: DBManager):
        self.db = db

    @route('', methods=['POST'])
    @expects_json(add_task_schema)
    def add_task(self):
        return jsonify({"code": 200, "msg": "created file transcoding task Successfully", "result": True})

    @route('/<taskId>', methods=['DELETE'])
    def cancel_task(self, taskId: str):
        return jsonify({"code": 200, "msg": "Delete file transcoding task Successfully", "result": True})

    @route('/<taskId>', methods=['GET'])
    def task_detail(self, taskId: str):
        return jsonify({"code": 200, "msg": "TODO JSON", "result": True})
