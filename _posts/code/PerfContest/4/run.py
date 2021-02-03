import sys
import subprocess
import os
import shutil
import argparse

# python run.py -v [-build] [-run] [-submission=<folder>]
# Instructions:
# The script goes through all the folders in current dir and tries 
# to build and run each submission.

parser = argparse.ArgumentParser(description='test submissions')
parser.add_argument("-v", help="verbose", action="store_true", default=False)
parser.add_argument("-build", help="only build", action="store_true", default=False)
parser.add_argument("-clean", help="do clean build", action="store_true", default=False)
parser.add_argument("-run", help="only run", action="store_true", default=False)
parser.add_argument("-submission", type=str, help="do single submission", default="")
args = parser.parse_args()

verbose = args.v
buildOnly = args.build
cleanBuild = args.clean
runOnly = args.run
buildAndRun = not runOnly and not buildOnly
doSubmission = args.submission

saveCwd = os.getcwd()

submissions = list(tuple())

if verbose:
  print ("Submissions:")

for submission in os.listdir(os.getcwd()):
  if not os.path.isfile(os.path.join(os.getcwd(), submission)):
    submissions.append((submission, os.path.join(os.getcwd(), submission)))
    if verbose:
      print ("  " + submission)

if buildOnly or buildAndRun:
  if verbose:
    print ("Building ...")

  for submissionName, submissionDir in submissions:
    if not doSubmission or (doSubmission and doSubmission == submissionName):

      if verbose:
        print ("Building " + submissionName + " ...")

      submissionBuildDir = os.path.join(submissionDir, "build")
      if cleanBuild and os.path.exists(submissionBuildDir):
        shutil.rmtree(submissionBuildDir)
 
      if not os.path.exists(submissionBuildDir):
        os.mkdir(submissionBuildDir)
      os.chdir(submissionBuildDir)    

      try:
	subprocess.check_call("cmake ../ > /dev/null", shell=True)
        print("  cmake - OK")
      except:
        print("  cmake - Failed")

      try:
        subprocess.check_call("make > /dev/null", shell=True)
        print("  make - OK")
      except:
        print("  make - Failed")

      inputFileName = str(os.path.join(saveCwd, "221575.pgm"))
      destDir = str(submissionBuildDir)
      try:
        subprocess.check_call("cp " + inputFileName + " " + destDir, shell=True)
        print("  copy image - OK")
      except:
        print("  copy image - Failed")

os.chdir(saveCwd)

if runOnly or buildAndRun:
  if verbose:
    print ("Running ...")

  scoretable = []

  baseline = float(0)

  for submissionName, submissionDir in submissions:
    if not doSubmission or (doSubmission and doSubmission == submissionName):

      if verbose:
        print ("Running " + submissionName + " ...")

      submissionBuildDir = os.path.join(submissionDir, "build")
      os.chdir(submissionBuildDir)
      scores = []

      inputFileName = str(os.path.join(submissionBuildDir, "221575.pgm"))
      runCmd = "./canny " + inputFileName + " 0.5 0.7 0.9 2>&1"
      valid = True
      try:
        subprocess.check_call(runCmd, shell=True)
      except:
        valid = False

      if valid:
        print("  run - OK")
      if not valid:
        print("  run - Failed")
      
      outputFileName = str(os.path.join(submissionBuildDir, "221575.pgm_s_0.50_l_0.70_h_0.90.pgm"))
      goldenFileName = str(os.path.join(saveCwd, "221575_golden.pgm"))

      valid = True
      try:
        subprocess.check_call("./validate " + goldenFileName + " " + outputFileName + " 2>&1", shell=True)
      except:
        valid = False
     
      if valid:
        print("  validation - OK")
      if not valid:
        print("  validation - Failed")

      for x in range(0, 10):
        output = subprocess.check_output("time -p " + runCmd, shell=True) 
        for row in output.split('\n'):
          if 'real' in row:
            real, time = row.split(' ')
            scores.append(float(time))

      copyScores = scores    
      copyScores.sort()
      minScore = copyScores[0]
      scoretable.append([minScore, submissionName, scores])
      if (submissionName == "canny_baseline"):
        baseline = minScore

  scoretable.sort()
  for score in scoretable:
      if (score[0] > 0):
        print(score, " + " + str(round((baseline / score[0] - 1) * 100, 2)) + "%")
      else:
        print(score, " + inf")
