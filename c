#!/usr/local/bin/php
<?php
class YouDaoApi
{
    const CURL_TIMEOUT = 10; 
    const URL = "https://fanyi-api.baidu.com/api/trans/vip/translate"; 
    const APP_ID = "20170710000063887";
    const SEC_KEY = "ckSBa0r5CPfB8qoNtF34";

    //翻译入口
    public function translate($query)
    {
        $args = array(
            'q' => $query,
            'appid' => self::APP_ID,
            'salt' => rand(10000,99999),
            'from' => 'auto',
            'to' => 'auto',
        );
        $args['sign'] = $this->buildSign($query, self::APP_ID, $args['salt'], self::SEC_KEY);
        $ret = $this->call(self::URL, $args);
        $ret = json_decode($ret, true);
        return $ret; 
    }

    public function buildSign($query, $appID, $salt, $secKey)
    {
        $str = $appID . $query . $salt . $secKey;
        $ret = md5($str);
        return $ret;
    }

    public function call($url, $args=null, $method="post", $testflag = 0, $timeout = self::CURL_TIMEOUT, $headers=array())
    {
        $ret = false;
        $i = 0; 
        while($ret === false) 
        {
            if($i > 1)
                break;
            if($i > 0) 
            {
                sleep(1);
            }
            $ret = $this->callOnce($url, $args, $method, false, $timeout, $headers);
            $i++;
        }
        return $ret;
    }

    public function callOnce($url, $args=null, $method="post", $withCookie = false, $timeout = CURL_TIMEOUT, $headers=array())
    {
        $ch = curl_init();
        if($method == "post") 
        {
            $data = $this->convert($args);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
            curl_setopt($ch, CURLOPT_POST, 1);
        }
        else 
        {
            $data = convert($args);
            if($data) 
            {
                if(stripos($url, "?") > 0) 
                {
                    $url .= "&$data";
                }
                else 
                {
                    $url .= "?$data";
                }
            }
        }
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_TIMEOUT, $timeout);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        if(!empty($headers)) 
        {
            curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        }
        if($withCookie)
        {
            curl_setopt($ch, CURLOPT_COOKIEJAR, $_COOKIE);
        }
        $r = curl_exec($ch);
        curl_close($ch);
        return $r;
    }

    public function convert(&$args)
    {
        $data = '';
        if (is_array($args))
        {
            foreach ($args as $key=>$val)
            {
                if (is_array($val))
                {
                    foreach ($val as $k=>$v)
                    {
                        $data .= $key.'['.$k.']='.rawurlencode($v).'&';
                    }
                }
                else
                {
                    $data .="$key=".rawurlencode($val)."&";
                }
            }
            return trim($data, "&");
        }
        return $args;
    }

}

class Application
{
    protected $_cmds;

    protected $_argv;

    public function __construct($argv)
    {
        $this->_argv = $argv;
    }

    public function commend($expression, $callable)
    {
        $cmd = explode(' ', $expression);
        if (isset($this->_cmds[$cmd[0]])) {
            throw new Exception("重复定义命令,$cmd[0]");
        }
        //注册
        $this->_cmds[$cmd[0]] = [
            'cmd' => $cmd,
            'callable' => $callable
        ];
    }

    public function run()
    {
        if (! isset($this->_cmds[$this->_argv[1]])) {
            throw new Exception("未知命令,$cmd[0]");
        }
        $myCmd = $this->_cmds[$this->_argv[1]];
        $params = $this->_argv;
        array_shift($params);
        array_shift($params);
        call_user_func_array($myCmd['callable'], $params);
    }
}

$app = new Application($argv);

$app->commend("d time", function($time){
    if (strpos($time, '-')) {
        $data = strtotime($time);
        echo $data . PHP_EOL;
    } else {
        $data = date("Y-m-d H:i:s", $time);
        echo $data . PHP_EOL;
    }
    return true;
});

$app->commend("c express", function($express){
    $evalStr = 'return ' . $express . ";";
    echo eval($evalStr);
    return true;
});

$app->commend("todo operate content", function($operate, $content){
    if ($operate == "add") {
        $content = "date: " . date("Y-m-d H:i:s", time()) . "\n" . $content ." \n";
        file_put_contents("./data/todo", $content . "\n", FILE_APPEND);
    }

    if ($operate == "list") {
        echo file_get_contents("./data/todo");
    }
    return true;
});

$app->commend("f word", function($word){
    $youDaoApi = new YouDaoApi();
    $info = $youDaoApi->translate($word);
    echo $info['trans_result'][0]['dst'] . PHP_EOL;
    return true;
});

$app->run();
