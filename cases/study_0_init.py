#!/usr/bin/env python 

from argparse import ArgumentParser, RawTextHelpFormatter
import sys
from subprocess import call, run
from shlex import quote
import os
import os.path


def set_up_dimensions (dim_1, dim_2): 
    """Renames 0/field.dim_1 to 0/field and removes 0/field.dim_2"""
    files_dim_1 = [os.path.join("0",fname) for fname in os.listdir("0") \
                if os.path.isfile(os.path.join("0", fname)) and \
                fname.endswith(dim_1)] 
    for file_dim_1 in files_dim_1:
        base_name = file_dim_1.rstrip("." + dim_1)
        call(["mv", file_dim_1, base_name])
        call(["rm", "-rf", base_name + "." + dim_2])

usage = """A wrapper for pyFoamRunParameterVariation.py that generates the
directory structure for a parameter study. 

Meshes are not generated and preprocessing is not done. 
Used to prepare for execution on a cluster.

Usage: ./create-study.py -c templateCase -p paramFile -s studyName"""

def parse_arguments():
    parser = ArgumentParser(description=usage, formatter_class=RawTextHelpFormatter)

    # parser.add_argument('-m', '--mesh',
    #                     choices=['hex', 'perturbed', 'poly'],
    #                     help="Meshtype: hexahedral, hexahedral-perturbed or polyhedral",
    #                     default='hex',
    #                     required=False,
    #                     dest='meshtype',
    #                     )

    parser.add_argument("-c", "--case", 
                    dest="casedir",
                    help="Template case directory.", 
                    required=True,
                    metavar="CASEDIR")

    parser.add_argument("-p", "--parameter-file", 
                    dest="paramfile", 
                    help="PyFoam parameter file used by pyFoamRunParameterVariation.py.", 
                    required=True,
                    metavar="PARAMFILE")

    parser.add_argument("-s", "--study-name", 
                    dest="studyname", 
                    help="Name of the parameter study.", 
                    required=True,
                    metavar="STUDYNAME")
    
    return parser.parse_args()

def create_studydir(studyname: str) -> str:
    studydir = f"study_{studyname}"
    try:
        os.mkdir(studydir)
    except FileExistsError:
        raise FileExistsError(f"Study directoy {studydir} already exists. Choose different studyname.")
    return studydir

def cp_templatecase_paramfile_to_studydir(args, studydir):
    run(f"cp -a {quote(args.casedir)} {args.paramfile} default.parameter {studydir}/",
        shell=True,
        check=True)
    
def create_parameterstudy(args):
    run([
        "pyFoamRunParameterVariation.py", "--no-execute-solver", "--no-server-process", 
        "--no-mesh-create", "--no-case-setup", "--cloned-case-prefix=%s" % args.studyname, 
        # f"--overload-directory={args.casedir}/scripts",
        "--parameter-file=default.parameter",
        "--every-variant-one-case-execution",
        "--create-database", args.casedir, args.paramfile
        ],
        check=True
    )

def create_pyFoam_variationfile(args, metaname):
    pyFoamVariation_file = metaname + '.pyFoam-variation'
    run(f"pyFoamRunParameterVariation.py --list-variation {quote(args.casedir)} {quote(args.paramfile)} > {pyFoamVariation_file}",
        shell=True, check=True
        )
    return pyFoamVariation_file

def create_json_variationfile(metaname, casesfile, pyfoamvariationfile):
    json_variationfile = metaname + '.json'
    run(f"case-parameter.py -l {casesfile} convert {pyfoamvariationfile} > {json_variationfile}",
        shell=True)
    return json_variationfile

    
def create_concrete_casesfile(args, metaname):
    casesfile = metaname + '.cases'
    run(f"find -maxdepth 1 -type d -name '{args.studyname}_[0-9][0-9][0-9][0-9][0-9]*' | sort | xargs -n1 basename > {casesfile}",
        shell=True, check=True)
    return casesfile

def create_infofile(metaname, studydir):
    infofile = metaname + '.info'
    cmd = f"""(
            echo "Study: {studydir}";
            echo "Study creation at:";
            date;
        ) > {infofile}"""
    run(cmd, shell=True, check=False)

# def get_concrete_cases(casesfile) -> list[str]:
#     with open(casesfile, 'r', encoding='utf-8') as file:
#         cases_ls = file.readlines()
#     return cases_ls
    
def cp_extrafiles_to_concrete_cases(extrafiles, args, casesfile):
    # extrafiles = [
    #     '*.stl',
    #     'Allrun*',
    # ]
    extrafiles = [os.path.join(args.casedir, file) for file in extrafiles]
    extrafiles_str = ' '.join(extrafiles)
    cmd = f"""
    cat {casesfile} | xargs -I[] bash -c 'cp {extrafiles_str} []/'
    """
    run(cmd, shell=True)
    

def main():
    args = parse_arguments()

    studydir = create_studydir(args.studyname)

    metaname = studydir

    # Copies the template case and parameter file into the study. Freezes information of all non-studyparameters to the study.
    cp_templatecase_paramfile_to_studydir(args, studydir)

    os.chdir(studydir)

    # Generate parameter study simulation cases with pyFoam
    create_parameterstudy(args)

    # Creates file which lists all concrete cases
    casesfile = create_concrete_casesfile(args, metaname)

    extrafiles = [
        '*.stl',
        'Allrun*.sbatch',
    ]
    cp_extrafiles_to_concrete_cases(extrafiles, args, casesfile)

    create_infofile(metaname, studydir)
    
    # Creates file with almost dictionary structure, connection the variation number to the parameter vector.
    pyFoam_variationfile = create_pyFoam_variationfile(args, metaname)
    
    # Creates file similiar to the pyFoamVariation_file, but with case basenames and in json format. Readable by python.
    create_json_variationfile(metaname, casesfile, pyFoam_variationfile)




if __name__ == "__main__":
    main()