import os
import subprocess
import json
import time
from config import masterQid, defaultJson

updateCode = 110
sleepCode = 109
quitCode = 0
sleepDuration = 3600 * 6

rootPath = os.getcwd() + '/' + os.path.dirname(__file__)
cqhttpAddr = '127.0.0.1'
cqhttpPort = 0
cqhttpSendPort = 0
cqhttpToken = ''
if not os.path.exists(rootPath + '/c-hakuBot/config.json'):
    if not os.path.exists(rootPath + '/c-hakuBot/hakuBot'):
        print('Where is haku?')
        exit()
    else:
        print('It seems that it\'s the first time to waken haku...')
        configFlag = 1
        if len(defaultJson) == 0:
            print('Couldn\'t load default json data.')
            configFlag = 0
        else:
            try:
                configDict = json.loads(defaultJson)
                cqhttpAddr = configDict['URL']
                configDict = json.loads(jsonData)
                cqhttpAddr = configDict['URL']
                cqhttpSendPort = configDict['SEND_PORT']
                cqhttpPort = configDict['PORT']
                cqhttpToken = configDict['TOKEN']
                masterTmpId = configDict['MASTER0']
                if cqhttpPort < 1024 or cqhttpPort > 49151 or cqhttpSendPort < 1024 or cqhttpSendPort > 49151:
                    print('Default json data is invalid.')
                    configFlag = 0
            except:
                print('Default json data is invalid.')
                configFlag = 0
        if not configFlag:
            print('You should edit haku\'s config file manually.')
            outPut = subprocess.getoutput(rootPath + '/c-hakuBot/hakuBot')
            #print(outPut)
            exit()
        else:
            jsonFile = open(rootPath + '/c-hakuBot/config.json')
            jsonFile.write(defaultJson)
            jsonFile.close();

jsonFile = open(rootPath + '/c-hakuBot/config.json')
jsonData = jsonFile.read()
jsonFile.close()
try:
    configDict = json.loads(jsonData)
    cqhttpAddr = configDict['URL']
    cqhttpSendPort = configDict['SEND_PORT']
    cqhttpPort = configDict['PORT']
    cqhttpToken = configDict['TOKEN']
    masterQid.append(configDict['MASTER0'])
    if cqhttpPort < 1024 or cqhttpPort > 49151 or cqhttpSendPort < 1024 or cqhttpSendPort > 49151:
        print('Port invalid?')
        print('Listen port:\t', cqhttpPort)
        print('Send port:\t', cqhttpSendPort)
        exit()
except Exception as e:
    print('haku config file load failed!')
    print(e)
    exit()

rtCode = subprocess.call('cd ' + rootPath + '/c-hakuBot/ && ./hakuBot')
while True:
    if rtCode == sleepCode:
        print('gen will wake up haku in ', sleepDuration, ' seconds.')
        time.sleep(sleepDuration)
    elif rtCode == quitCode:
        print('Quit gen.')
        break
    elif rtCode == updateCode:
        gitPull = subprocess.getoutput('cd ' + rootPath + '/c-hakuBot/ && git pull')
        makeInstall = subprocess.getoutput('cd ' + rootPath + ' && make && make install && make clean')
        print(gitPull)
        print(makeInstall)
    else:
        print('Unknown return code. Try to waken haku.', rtCode)
    rtCode = subprocess.call('cd ' + rootPath + '/c-hakuBot/ && ./hakuBot')





