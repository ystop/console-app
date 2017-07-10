#!/usr/local/bin/php
<?php
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

$app->run();
