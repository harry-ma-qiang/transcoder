# Import the framework
from flask import Flask, jsonify, request
from flask_classful import FlaskView, route
from flask_expects_json import expects_json
from DbManagers.db_manager import DBManager

# Validation schemas
add_wall_schema = {
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
        "wallId": {"type": "string"},
        "wallSource":  {
            "type": "array",
            "minItems": 1,
            "items": {"type": "string"}
        }
    },
    "additionalProperties": False,
    "required": ["wallId", "wallSource"]
}

update_wall_schema = {
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
        "wallSource":  {
            "type": "array",
            "minItems": 1,
            "items": {"type": "string"}
        }
    },
    "additionalProperties": False,
    "required": ["wallSource"]
}


class WallView(FlaskView):
    def __init__(self, db: DBManager):
        self.db = db

    @route('', methods=['POST'])
    @expects_json(add_wall_schema)
    def add_wall(self):
        return jsonify({"code": 200, "msg": "create TVWall success", "result": True})

    @route('/<wallId>', methods=['POST'])
    @expects_json(update_wall_schema)
    def update_wall(self, wallId: str):
        return jsonify({"code": 200, "msg": "update TVWall successfully", "result": True})

    @route('/<wallId>', methods=['DELETE'])
    def delete_wall(self, wallId: str):
        return jsonify({"code": 200, "msg": "Delete TVWall successfully", "result": True})
