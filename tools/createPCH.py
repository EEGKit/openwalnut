#!/usr/bin/env python
#
# little helper script for OpenWalnut to automaticly create PCH files for faster recompile
# By Robin Ledig - Oct 2010

import sys
import os
import string
import os.path
import re

#todo: if a 0 is specified for addPCHtoCPP clear all the generated PCH files from the modules directorys

HELP_USAGE = "Usage: createPCH.py moduleName filelist.txt [1:addPCHtoCPP]\n"
PY_SCRIPT_CREATED = "// these lines where created by a python script, do not edit"
PCH_INCLUDE_LINES = 2

def fileWithoutPath(filename):
    found = string.rfind(filename,"/")
    if ( found == -1 ):
        return filename
    return filename[found+1:]

class FileType:
    def __init__(self,filenameWithPath):
        self.fileWithPath = filenameWithPath
        self.file = fileWithoutPath(filenameWithPath)
        
    fileWithPath = ""
    file = ""
    
class IncludeGuards:
    def __init__(self,line):
        self.includeLine = line
        tmpStr = ""
        p1 = string.find(line,"<")
        if ( p1 != -1 ):
            tmpStr = line[p1+1:string.find(line,">")]
        else:
            p1 = string.find(line,"\"")
            tmpStr = line[p1+1:string.rfind(line,"\"")]
        self.includeFile = FileType(tmpStr)
        self.preGuardLines = []
        self.postGuardLines = []
        
    includeFile = FileType("")
    includeLine = ""
    preGuardLines = []
    postGuardLines = []

#read a complete file into a stringlist
def read_file (filename):
    usefn = filename
    pos = string.find(usefn,"\n") 
    if ( pos >= 0 ):
        usefn = filename[:pos]

    if not os.path.exists(usefn):
        print "File " + usefn + " doesn't exist."
        return ()
        
    f = file(usefn, "r")
    lines = f.readlines()
    f.close()
    for i in range(len(lines)):
        lines[i] = string.rstrip(lines[i],"\n")
    return lines

def getModulePCHFN(modulename):
    return modulename+"_PCH.h"
    
#also consider:
#we are in a subpath e.g. datahandler/io
# and include biosig/win
#this wont be found in pch in datahandler
#->if file with path exists in folder from where the incline comes
#and the incline is not directly in the pch folder, add the inline folder before the including file
def checkRelativePathExistance(filename,incline):
    if ( string.find(incline.includeFile.fileWithPath,"../") == -1 ):
        pos = string.rfind(filename.fileWithPath,"/")
        path = filename.fileWithPath
        if ( pos > -1 ):
            path = filename.fileWithPath[:pos+1]
        if os.path.exists(path+incline.includeFile.fileWithPath):
            newpath = path+incline.includeFile.fileWithPath

            line = incline.includeLine
            start = string.find(line,"<")
            end = string.find(line,">")
            if ( start < 0 ):
                start = string.find(line,"\"")
                end = string.rfind(line,"\"")
            newline = line[:start+1]
            newline = newline+newpath+line[end:]
            return IncludeGuards(newline)
    
        return incline
    if os.path.exists(incline.includeFile.fileWithPath):
        return incline
    else:
        newpath = incline.includeFile.fileWithPath
        while ( string.find(newpath,"../") == 0 ):
            newpath = newpath[3:]
            if os.path.exists(newpath):
                line = incline.includeLine
                start = string.find(line,"<")
                end = string.find(line,">")
                if ( start < 0 ):
                    start = string.find(line,"\"")
                    end = string.rfind(line,"\"")
                newline = line[:start+1]
                newline = newline+newpath+line[end:]
                return IncludeGuards(newline)
    return incline
            
    
#determine PCH include list
#
#first read the files we use and add them to a list of FileType (V)
#create a new empty FileType list (V)
#iterate over that list (V)
#  open every file (V)
#  reset current stack (V)
#  iterate over the lines (V)
#  on ifdef and such things push or pop current stack
#  on include, check if the include file is in our modulefilelist
#   if not add it to the list of includes for PCH
#
# NOTE: exclude pchfilename itself
#

def includeListForPCH(moduleListFile,modulename):
    filesLine = read_file(moduleListFile)
    usedFiles = []
    includes = []
    for file in filesLine:
        usedFiles.append(FileType(file))
    
    #iteration over all source files
    for sourcefile in usedFiles:
        if (string.lower(sourcefile.file) == string.lower(getModulePCHFN(modulename))):
            continue
        ifdefStack = []
        sourcefileLines = read_file(sourcefile.fileWithPath)
        first = 1
        isHeader = 0
        registeredInputLines = []
        #this will help to ignore the include guard of the header file which will be there all the time
        if ( string.find(sourcefile.file,".h") >= 0 ):
            isHeader = 1
        #iteration over all lines of a source file
        for line in sourcefileLines:
            #encountering an #if means an #ifdef sort of block is beginning
            if ( string.find(line, "#if") >= 0 ):
                if (isHeader and first):
                    first = 0
                else:
                    ifdefStack.append([line])
                    
            if ( string.find(line,"#el") >= 0 ):
                ifdefStack[len(ifdefStack)-1].append(line)
                
            # pop the last stack and append that line to the registered include lines
            if ( string.find(line,"#endif") >= 0 ):
                if ( len(registeredInputLines) > 0 ):
                    for index in registeredInputLines:
                        includes[index].postGuardLines.append(line)
                if ( len(ifdefStack) > 0 ):
                    ifdefStack.pop()
                if ( len(ifdefStack) == 0 ):
                    registeredInputLines = [] 
                    
            #include found
            if ( string.find(line,"#include") >= 0 ):
                #create a include instance
                incline = IncludeGuards(line)
                if ( string.find(incline.includeFile.file,".") >= 0 ):
                    if ( string.find(incline.includeFile.file,".h") == -1 and string.find(incline.includeFile.file,".hpp") == -1 ):
                        continue
                
                #check if this input is already in our list
                found = 0
                for alreadyincluded in includes:
                    if ( string.lower(alreadyincluded.includeFile.file) == string.lower(incline.includeFile.file) ):
                        found = 1
                #found will also be true if the file is our pch file
                if ( string.lower(getModulePCHFN(modulename)) == string.lower(incline.includeFile.file) ):
                    found = 1
                for modulefile in usedFiles:
                    if ( modulefile.file == incline.includeFile.file ):
                        found = 1
                
                #if not prepend the stack and add the file to the list
                if ( found == 0 ):
                    incline = checkRelativePathExistance(sourcefile, incline)
                    for stacklines in ifdefStack:
                        for ifdefs in stacklines:
                            incline.preGuardLines.append(ifdefs)
                    includes.append( incline )

                    #register it to the stack
                    if ( len(ifdefStack) > 0 ):
                        registeredInputLines.append( len(includes)-1 )
            
            #a beginning class or block means we should be finished with this files includes
            if ( string.find(line,"class") >= 0 or string.find(line,"{") >= 0 ):
                #this may cause an unexpected error doe to an endif guarding an include at the end of a file
                #so go through the registered files and add them an #endif as often as the count of the stacksize
                if ( len(ifdefStack) > 0 ):
                    for i in ifdefStack:  
                        if ( len(registeredInputLines) > 0 ):
                            for index in registeredInputLines:
                                includes[index].postGuardLines.append("#endif")                                   
                break
    
    #loop ended
    return includes
    
def writeFile(filename,lines):
    for i in range(len(lines)):
        lines[i] = string.rstrip(lines[i],"\n")
        lines[i] = lines[i] + "\n"
    f = open(filename, "w")
    f.writelines(lines)
    f.close()

def writeFileIfNotExistantOrDifferent(filename,lines):
    if ( not os.path.exists(filename) ):
        writeFile(filename,lines)
        return
    targetFile = read_file(filename)
    #unequal line count -> overwrite the file
    if ( len(lines) != len(targetFile) ):
        writeFile(filename,lines)
        return
    identical = 1
    for i in range(len(lines)):
        if ( string.find(lines[i],targetFile[i]) == -1 ):
            identical = 0
            break
    if ( identical == 0 ):
        writeFile(filename,lines)
    

#write pch file:
#
#obvious :)
#
#add pch to all cpp's in filelist
#
#iterate over all files in the source file list and only use the cpp files
#read the complete file
#create a new temporary stringlist for the file
#read the file until end of copyright is reached while copying the lines to the new list
#check if the pch is included
#further act on the argument flag, either keep it this way, remove or add it
#write the rest of the file to the temporary string list
#write the stringlist to the file

def writePCH(filename,includes,modulename):
    out = []
    out.append("#ifndef "+modulename+"_PCH_H")
    out.append("#define "+modulename+"_PCH_H")
    out.append("\n")
    
    for include in includes:
        ifGuard = 0
        if ( len(include.preGuardLines) > 0 ):
            ifGuard = 1
        if ( ifGuard == 1 ):
            for guardline in include.preGuardLines:
                out.append(guardline)
        out.append(include.includeLine)
        if ( ifGuard == 1 ):
            for guardline in include.postGuardLines:
                out.append(guardline)

    out.append("\n")
    out.append("#endif // "+modulename+"_PCH_H")
    
    slash = string.rfind(filename,"/")
    dir = filename
    if ( slash >= 0 ):
        dir = filename[:slash]

    dir += getModulePCHFN(modulename)
        
    dot = string.find(dir,".")
    pchhFilename = dir+".h"
    pchcppFilename = dir+".cpp"
    if ( dot >= 0 ):
        pchhFilename = dir[:dot]+".h"
        pchcppFilename = dir[:dot]+".cpp"
        
    for i in range(len(out)):
        out[i] = out[i]+"\n" 
    writeFileIfNotExistantOrDifferent(pchhFilename, out)
#    f = open(pchhFilename, "w")
#    f.writelines(out)
#    f.close()

    out = []
    out.append(PY_SCRIPT_CREATED)
    out.append("#include \""+getModulePCHFN(modulename)+"\"")
    
    for i in range(len(out)):
        out[i] = out[i]+"\n" 
    #print "writing "+pchcppFilename
    writeFileIfNotExistantOrDifferent(pchcppFilename, out)
#    f = open(pchcppFilename, "w")
#    f.writelines(out)
#    f.close()
    
# check where the pch file lays in relative to the current path
# Note this does not realy work, lets use the approch proposed from microshit:
# - include the pch file is if it where in the current directory
# - create a file with the given name in the current directory (if not already existing)
#   containing just the include to the original pch file e.g. #include "../mymodule.h"
def modulePCHFNRel(modulename,filename):
    propModuleFilename = getModulePCHFN(modulename)
    slash = string.rfind(filename,"/")
    dir = filename
    fn = propModuleFilename
    if ( slash != -1 ):
        dir = filename[:slash+1]
    while ( not os.path.exists(dir+fn) ):
        fn = "../"+fn
       
    if ( not os.path.exists(dir+propModuleFilename) ):
        out = []
        out.append("#include \""+fn+"\"")
        f = open(dir+propModuleFilename, "w")
        f.writelines(out)
        f.close()
       
    return propModuleFilename
    
def addPCHIncludeToCpps(moduleListFile,modulename,addPCHtoCPP):
    filesLine = read_file(moduleListFile)
    #iterate over all files in the file list
    for file in filesLine:
        #only modify cpp files if necessary
        if ( string.find(file,".cpp") >= 0 ):
            cpplines = read_file(file)
            newfileLines = []
            incpyrghtcmt = 1
            ssmmcount = 0
            ispygenblock = 0
            pylinecount = 0
            lineafterprecode = 0
            pygenblockfound = 0
            #parse lines of the cpp
            cppmodified = 0
            for lines in cpplines:
                #did we just leave the comment block and encounter the script line
                if ( incpyrghtcmt == 0 and string.find(lines,PY_SCRIPT_CREATED) != -1 ):
                    ispygenblock = 1

                
                if ( incpyrghtcmt == 0 ):
                    lineafterprecode += 1
                
                #when we are in the first line after the copyright block and its not the python script
                #add the pch header include if wished
                if ( lineafterprecode == 1 and ispygenblock == 0 and addPCHtoCPP == 1 ):
                    newfileLines.append(PY_SCRIPT_CREATED)
                    newfileLines.append("#include \""+modulePCHFNRel(modulename,file)+"\"")
                    #newfileLines.append("#include \""+getModulePCHFN(modulename)+"\"")
                    cppmodified = 1
                
                #count the number of comment begin and end to determine when the code block starts
                if ( string.find(lines,"//--") >= 0 ):
                    ssmmcount += 1
                if ( ssmmcount >= 2 and incpyrghtcmt == 1 ):
                    incpyrghtcmt = 0
                                    
                #if the pygenblock is found increase there line count
                if ( ispygenblock == 1 ):
                    pylinecount += 1
                    if ( pylinecount > PCH_INCLUDE_LINES ):
                        ispygenblock = 0

                #just copy the line if we are not in the pygenblock or if in it and want to keep it
                if ( ispygenblock == 0 or ( ispygenblock == 1 and addPCHtoCPP == 1 ) ):
                    newfileLines.append(lines)
                else:
                    cppmodified = 1
            
            #only write the file if it got modified
            if ( cppmodified == 1 ):
                f = open(file, "w")
                for i in range(len(newfileLines)):
                    newfileLines[i] = newfileLines[i]+"\n" 
                f.writelines(newfileLines)
                f.close()
                
def removePCH(moduleListFile,modulename):
    name = getModulePCHFN(modulename)
        
    dot = string.find(name,".")
    pchhFilename = name+".h"
    pchcppFilename = name+".cpp"
    if ( dot >= 0 ):
        pchhFilename = name[:dot]+".h"
        pchcppFilename = name[:dot]+".cpp"

    filesLine = read_file(moduleListFile)
    #iterate over all files in the file list
    for file in filesLine:
        slash = string.rfind(file,"/")
        dir = file
        if ( slash != -1 ):
            dir = file[:slash+1]
        if ( os.path.exists(dir+pchhFilename) ):
            os.remove(dir+pchhFilename)
        if ( os.path.exists(dir+pchcppFilename) ):
            os.remove(dir+pchcppFilename)
       
                
def main(argv):
    #print "main"
    
    num_args = len(sys.argv)-1
    if num_args < 2 or num_args > 3:
        print HELP_USAGE
        #print string(num_args)
        return
        
    modulename = sys.argv[1]
    listfile = sys.argv[2]
    #pchfn = sys.argv[3]
    pchfn = getModulePCHFN(modulename)
        
    if num_args == 3:
        addPCH = int(sys.argv[3])
    else:
        addPCH = 1

    #get the includes
    #print "reading include list"
    #print "args:"
    #for arg in sys.argv:
    #    print arg    
    
    includes = includeListForPCH(listfile,modulename)
    #write the pch
    writePCH(pchfn,includes,modulename)
    #update the cpp's
    addPCHIncludeToCpps(listfile,modulename,addPCH)
    
    if ( addPCH == 0 ):
        removePCH(listfile,modulename)
    
if __name__ == "__main__":
    main( sys.argv )
    
