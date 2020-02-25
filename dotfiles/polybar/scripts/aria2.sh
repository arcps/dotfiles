#!/usr/bin/env sh

host='http://192.168.0.2'
port=''
secret=''

doRequest() {
    request="$(aria2p -H $host call getGlobalStat 2> /dev/null)"
    status=$?
    echo "$request"
    return $status
}

main() {
    # if no args, keeps printing the current speed
    if [ -z "$1" ]; then {
        while [ true ]
        do
            requestDid="$(doRequest)";

            # verify if requisition was successful
            [[ $? -eq 0 ]] && isConnected="true" || isConnected="false"

            if [ "$isConnected" == "true" ]; then
                ariaDownloadSpeed="$(echo $requestDid | jq -r .downloadSpeed)"

                echo "Aria2: $(($ariaDownloadSpeed/1024))KB/s ";
                sleep 1.5;
            else
                echo "";
                sleep 10;
            fi

        done
    } else
        case "$1" in
            "click")
                speed="$(rofi -dmenu -lines 0 -p 'Define Aria2 download speed')"
                defineAriaDownSpeed "$speed"
            ;;
        esac
    fi
}

main "$@"; exit
