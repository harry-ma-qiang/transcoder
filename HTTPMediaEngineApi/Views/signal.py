# Import the framework
import json
from flask import Flask, jsonify, request
from flask_classful import FlaskView, route
from flask_expects_json import expects_json
from DbManagers.db_manager import DBManager

# Validation schemas
add_signal_schema = {
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
        "signalId": {"type": "string"},
        "signalSource": {"type": "string"},
        "programId": {"type": "number"},
        "enableTranscode": {"type": "boolean"},
        "config": {
            "type": "array",
            "minItems": 1,
            "maxItems": 1,
            "items":
            {
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
            }
        },
        "hls": {
            "type": "object",
            "properties": {
                "segTime": {"type": "number"},
                "segNum": {"type": "number"}
            },
            "additionalProperties": False,
            "required": ["segTime", "segNum"]
        }
    },
    "additionalProperties": False,
    "required": ["signalId", "signalSource", "programId", "enableTranscode", "hls", "config"]
}

update_signal_schema = {
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
        "signalSource": {"type": "string"},
        "programId": {"type": "number"},
        "enableTranscode": {"type": "boolean"},
        "config": {
            "type": "array",
            "minItems": 1,
            "maxItems": 1,
            "items":
            {
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
            }
        },
        "hls": {
            "type": "object",
            "properties": {
                "segTime": {"type": "number"},
                "segNum": {"type": "number"}
            },
            "additionalProperties": False,
            "required": ["segTime", "segNum"]
        }
    },
    "additionalProperties": False,
    "required": ["signalSource", "programId", "enableTranscode", "hls", "config"]
}


class SignalView(FlaskView):
    def __init__(self, db: DBManager):
        self.db = db

    def action(self):
        return jsonify({"code": 200, "msg": "start success", "result": True})

    @route('', methods=['POST'])
    @expects_json(add_signal_schema)
    def add_signal(self):
        signal_object = request.get_json(force=True)
        self.db.add_signal(signal_object)
        return jsonify({"code": 200, "msg": "create signal success", "result": True})

    @route('/<signalId>', methods=['POST'])
    @expects_json(update_signal_schema)
    def update_signal(self, signalId: str):
        signal_object = request.get_json(force=True)
        signal_object.update({'signalId': signalId})
        self.db.update_signal(signal_object)
        return jsonify({"code": 200, "msg": "Update signal config success", "result": True})

    @route('/<signalId>', methods=['DELETE'])
    def delete_signal(self, signalId: str):
        self.db.delete_signal(signalId)
        return jsonify({"code": 200, "msg": "Delete signal successfully", "result": True})