#!/usr/bin/env sh

fastBackward='%{A1:playerctl -p vlc previous:}%{A} '
fastForward=' %{A1:playerctl -p vlc previous:}%{A}'
totalParts=32;

main() {
    status="$(playerctl -p vlc status)"
    if [[ "$status" == "Stopped" || "$status" == "No players found" ]]; then echo "" && exit 0; fi

    musicLength="$(playerctl -p vlc metadata vlc:time)"
    partLength="$(($musicLength/$totalParts))"

    if [ -z "$1" ]; then
        seekBar=" "
        case "$status" in
            "Playing")
                playPause="%{A1:playerctl -p vlc play-pause:}%{A}";
                createSeekBar
                echo -e "  $fastBackward $playPause $fastForward %{B#44000000}$seekBar%{B#00000000}    "
                ;;
            "Paused")
                playPause="%{A1:playerctl -p vlc play-pause:}%{A}";
                createSeekBar
                echo -e "  $fastBackward $playPause $fastForward %{B#44000000}$seekBar%{B#00000000}    "
                ;;
        esac
    elif [[ -n "$2" && "$1" == "seek" ]]; then

        position="$2";

        finalPosition="$(($partLength*$position))"
        playerctl -p vlc position "$finalPosition"

    fi
}


function createSeekBar () {
    currentPosition="$(playerctl -p vlc position)"
    currentPosition="${currentPosition%.*}"
    
    for (( i=1; i<=$totalParts; i++ ))
    do
        thisPartTime=$(($partLength*$i))
        if [ $thisPartTime -le $currentPosition ]; then
            seekBar="$seekBar%{A1:~/.config/polybar/scripts/player.sh seek $i:}%{F#C83143}-%{A}"
        else
            seekBar="$seekBar%{A1:~/.config/polybar/scripts/player.sh seek $i:}%{F#00FFFFF}-%{A}"
        fi
    done
}

main "$@"; exit
