"""
import urllib3


def RequestWebpage(webpage_link):
    print("Request webpage: " + webpage_link)
    http = urllib3.PoolManager()
    request = http.request("GET", webpage_link)
    print("Webpage content: " + request)
"""

import time

import requests

def RequestWebpage(webpage_link, debugprint=False,  timeout_sec=1):
    try:
        response = requests.get(webpage_link, timeout=timeout_sec)
        #print (response.status_code)
        if debugprint:
            print(response.content)
        return response.content
    except:
        return None


def RequestWebpageMassively(webpage_link, request_num=100, periodtime_sec=1, timeout_sec=1, debug=True):
    print("Request this webpage: \"{}\", {} times, {}[ms] periodtime, timeout: {}".format(webpage_link, request_num, periodtime_sec, timeout_sec))
    
    success_num = 0
    for i in range(0, request_num):
        content = RequestWebpage(webpage_link, timeout_sec=timeout_sec)
        is_ok = False
        
        if content is not None:
            is_ok = True
            success_num += 1
        
        if debug:
            if is_ok:
                print("+", end="", flush=True)
            else:
                print("-", end="", flush=True)

        time.sleep(periodtime_sec)
            
    # End of test
    print("")
    print("Result: {} successful, {} failed".format(success_num, request_num-success_num))


if __name__ == "__main__":
    #RequestWebpage("https://wwww.python.org/")
    #RequestWebpage("http://192.168.1.11:2000/index.html")
    RequestWebpageMassively("http://192.168.1.11/index.html", request_num=100, periodtime_sec=1.0)
