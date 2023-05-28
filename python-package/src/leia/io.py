import json
import yaml


def read_yaml(file) -> dict:
    with open(file,'r', encoding='utf-8') as f:
        info = yaml.safe_load(f)
    return info

def write_yaml(data, file) -> None:
    assert isinstance(data, dict)
    with open(file,'w', encoding='utf-8') as f:
        yaml.dump(data, f)

def read_json(file) -> dict:
    with open(file,'r', encoding='utf-8') as f:
        info = json.load(f)
    return info

def write_json(data, file) -> None:
    assert isinstance(data, dict)
    with open(file,'w', encoding='utf-8') as f:
        json.dump(data, f)

def read_list(file) -> list:
    with open(file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    if len(lines) > 1:
        # sep = '\n'
        return lines
    else:
        # sep = 'whitespace'
        return lines.split()

def read_info(infofile) -> dict:
    return read_yaml(infofile)

def write_info(data, infofile) -> None:
    write_yaml(data, infofile)

def read_cases(file):
    """
    Reads cases from JSON or list file.
    """
    try:
        return read_json(file).keys()
    except:
        return read_list(file)