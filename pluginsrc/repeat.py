def func (msgDict):
    req = list(msgDict['raw_message'].split(' ', 1))
    if len(req) > 1:
        req[1] = req[1].strip()
        if len(req[1]) == 0: return
    else:
        return

    return req[1]
 
