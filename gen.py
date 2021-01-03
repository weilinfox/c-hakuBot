import os
import subprocess
import json
import time
import requests
from config import masterQid, defaultJson

updateCode = 110
sleepCode = 109
quitCode = 0
sleepDuration = 3600 * 6

rootPath = os.getcwd() + '/' + os.path.dirname(__file__)
cqhttpAddr = '127.0.0.1'
cqhttpPort = 0
cqhttpSendPort = 0
cqhttpSendPortStr = ''
cqhttpPortStr = ''
cqhttpToken = ''
if not os.path.exists(rootPath + '/c-hakuBot/config.json'):
    if not os.path.exists(rootPath + '/c-hakuBot/hakuBot'):
        print('Build haku~')
        outPut = subprocess.getoutput('cd ' + rootPath + ' && make && make install && make clean')
        print(outPut)
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
                print(defaultJson)
                configDict = json.loads(defaultJson)
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
            outPut = subprocess.getoutput('cd ' + rootPath + '/c-hakuBot && ./hakuBot')
            print(outPut)
            exit()
        else:
            jsonFile = open(rootPath + '/c-hakuBot/config.json', 'w')
            jsonFile.write(defaultJson)
            jsonFile.close();

jsonFile = open(rootPath + '/c-hakuBot/config.json')
jsonData = jsonFile.read()
jsonFile.close()
try:
    configDict = json.loads(jsonData)
    cqhttpAddr = configDict['URL']
    cqhttpSendPort = configDict['SEND_PORT']
    cqhttpSendPortStr = str(cqhttpSendPort)
    cqhttpPort = configDict['PORT']
    cqhttpPortStr = str(cqhttpPort)
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

rtCode = subprocess.call('./hakuBot', shell = True, cwd = rootPath + '/c-hakuBot/')
while True:
    if rtCode == sleepCode:
        print('\ngen will wake up haku in ', sleepDuration, ' seconds.\n')
        time.sleep(sleepDuration)
    elif rtCode == quitCode:
        print('\nQuit gen.\n')
        break
    elif rtCode == updateCode:
        print('\nStart update process.')
        sendMsgUrl = ''
        sendMsgParms = {}
        sendMsgFlag = True
        if os.path.exists(rootPath + '/c-hakuBot/quit.txt'):
            quitFile = open(rootPath + '/c-hakuBot/quit.txt')
            quitMsg = quitFile.read()
            quitFile.close()
            quitMsgList = list(quitMsg.split())
            try:
                quitDict = json.loads(quitMsgList[len(quitMsgList)-1])
                if quitDict['post_type'] == 'message':
                    if quitDict['message_type'] == 'private':
                        sendMsgUrl = 'http://' + cqhttpAddr + ':' + cqhttpSendPortStr + '/send_private_msg'
                        sendMsgParms = {'user_id':quitDict['user_id'], 'message':'', 'auto_escape':False}
                        if len(cqhttpToken): sendMsgParms.update({'access_token':cqhttpToken})
                    elif quitDict['message_type'] == 'group':
                        sendMsgUrl = 'http://' + cqhttpAddr + ':' + cqhttpSendPortStr + '/send_group_msg'
                        sendMsgParms = {'group_id':quitDict['group_id'], 'message':'', 'auto_escape':False}
                        if len(cqhttpToken): sendMsgParms.update({'access_token':cqhttpToken})
                    else:
                        sendMsgFlag = False
                        print('Unsupported message. ', quitDict['message_type'])
                else:
                    sendMsgFlag = 0;
                    print('Quit message is not a message event?')
            except Exception as e:
                sendMsgFlag = False
                print('Gen parse quit message Failed')
                print(e)
        else:
            sendMsgFlag = False
            print('No quit message?')
                
        gitPull = subprocess.getoutput('cd ' + rootPath + '/c-hakuBot/ && git pull')
        if sendMsgFlag:
            sendMsgParms['message'] = 'Git returned message:\n' + gitPull;
            resp = requests.get(url=sendMsgUrl, params=sendMsgParms)
            print(resp)
        makeInstall = subprocess.getoutput('cd ' + rootPath + ' && make')
        subprocess.getoutput('cd ' + rootPath + ' && make install && make clean')
        makeList = list(makeInstall.split('\n', makeInstall.count('\n')))
        #print(makeList)
        errorFlag = False
        sendFlag = False
        for s in makeList:
            #if errorFlag:
            #    errorFlag = False
            #    sendFlag = True
            #    sendMsgParms['message'] = s
            #    resp = requests.get(url=sendMsgUrl, params=sendMsgParms)
            #    print(resp)
            if 'warning ' in s or 'error ' in s or 'fatal' in s or 'Warning ' in s or 'Error ' in s or 'Fatal' in s or \
                'errors' in s or 'Errors' in s or 'wainings' in s or 'Warnings' in s or \
                'error: ' in s or 'Error: ' in s or 'warning:' in s or 'Warning:' in s:
                errorFlag = True
                sendFlag = True
                sendMsgParms['message'] = s
                resp = requests.get(url=sendMsgUrl, params=sendMsgParms)
                print(resp)

        if not sendFlag:
            sendMsgParms['message'] = 'No errors, no warnings.'
            resp = requests.get(url=sendMsgUrl, params=sendMsgParms)
            print(resp)
        print('Finished. Wake haku up now.\n')
        #print(gitPull)
        #print(makeInstall)
    else:
        print('\nUnknown return code. Try to waken haku.\n', rtCode)
    rtCode = subprocess.call('./hakuBot', shell = True, cwd = rootPath + '/c-hakuBot/')





