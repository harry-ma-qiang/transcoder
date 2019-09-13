# Import the framework
from flask import Flask, jsonify, request
from flask_classful import FlaskView, route
from flask_expects_json import expects_json
from DbManagers.db_manager import DBManager

update_license_schema = {
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
        "license": {"type": "string"}
    },
    "additionalProperties": False,
    "required": ["license"]
}


class LicenseView(FlaskView):
    def __init__(self, db: DBManager):
        self.db = db

    @route('', methods=['POST'])
    @expects_json(update_license_schema)
    def update_license(self):
        return jsonify({"code": 200, "msg": "Update license", "result": True})

    @route('', methods=['GET'])
    def license_detail(self):
        return jsonify({"code": 200, "msg": "TODO JSON", "result": True})
