import requests
import json

def main (msgDict):
    helpMsg = '小白会试着从qq音乐搜索~'
    req = list(msgDict['raw_message'].split(' ', 1))
    ans = ''
    if len(req) > 1:
        req[1] = req[1].strip()
    if len(req) > 1 and len(req[1]) > 0:
        url = 'https://c.y.qq.com/soso/fcgi-bin/client_search_cp'
        params = {
            'ct': 24,
            'qqmusic_ver': 1298,
            'new_json': 1,
            'remoteplace': 'txt.yqq.song',
            'searchid': '',
            't': 0,
            'aggr': 1,
            'cr': 1,
            'catZhida': 1,
            'loseless': 0,
            'flag_qc': 0,
            'p': 1,
            'n': 20,
            'w':req[1]
        }
        try:
            resp = requests.get(url=url,params=params)
            if resp.status_code == 200:
                rejson = json.loads(list(resp.text.split('callback('))[1][:-1])
                # print(rejson)
                if rejson['data']['song']['totalnum'] == 0:
                    ans = '好像啥也没找到umm'
                else:
                    mscid = rejson['data']['song']['list'][0]['mid']
                    mscname = rejson['data']['song']['list'][0]['name']
                    ans =  '[CQ:share,url=https://y.qq.com/n/yqq/song/' + str(mscid) + '.html,title=' + str(mscname) + ']'
            else:
                ans = '好像返回了奇怪的东西: ' + str(resp.status_code)
        except Exception as e:
            ans = '啊嘞嘞好像出错了，一定是疼讯炸了不关小白！'
    else:
        ans = helpMsg

    return ans
 
