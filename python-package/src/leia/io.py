# import json
import yaml

# def read_info(infofile) -> dict:
#     with open(infofile,'r', encoding='utf-8') as f:
#         info = json.load(f)
#     return info

# def write_info(data, infofile) -> None:
#     assert isinstance(data, dict)
#     with open(infofile,'w', encoding='utf-8') as f:
#         json.dump(data, f)

def read_info(infofile) -> dict:
    with open(infofile,'r', encoding='utf-8') as f:
        info = yaml.safe_load(f)
    return info

def write_info(data, infofile) -> None:
    assert isinstance(data, dict)
    with open(infofile,'w', encoding='utf-8') as f:
        yaml.dump(data, f)