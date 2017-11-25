let currentTrackDate;
let newTrackDate;
let map;

function getTrack() {
    const time = Math.floor(currentTrackDate / 1000);
    $.getJSON("/api/track?time=" + time).done((data) => {
        const trackCollection = new ymaps.GeoObjectCollection();
        const points = [];
        data.forEach((item) => {
            const point = [item.latitude, item.longitude];
            trackCollection.add(new ymaps.Placemark(point, {
                hintContent: item.latitude + ", " + item.longitude,
                balloonContent: 'Время: ' + item.time
            }));
            trackCollection.add(new ymaps.GeoObject({
                geometry: {
                    type: "Circle",
                    coordinates: point,
                    radius: item.accuracy
                }
            }));
            points.push(point);
        });

        if (points.length > 0) {
            const polyline = new ymaps.Polyline(points);
            map.geoObjects.add(trackCollection);
            map.geoObjects.add(polyline);
            map.setBounds(polyline.geometry.getBounds());
        }
    }).fail(() => {
        Materialize.toast('Произошла ошибка при загрузке данных', 10000, 'rounded');
    });
}

function initPicker() {
    const picker = $('.datepicker').pickadate({
        selectMonths: true, // Creates a dropdown to control month
        selectYears: 15, // Creates a dropdown of 15 years to control year,
        today: 'Today',
        clear: 'Clear',
        close: 'Ok',
        closeOnSelect: false, // Close upon selecting a date,
        defaultDate: new Date(),
        onSet: (data) => {
            newTrackDate = data.select;
        },
        onClose: () => {
            if (currentTrackDate !== newTrackDate) {
                currentTrackDate = newTrackDate;
                getTrack();
            }
        }
    });

    let now = new Date();
    picker.pickadate('picker').set('select', now);
    currentTrackDate = now.getTime();
}

if (window.location.pathname === '/') {
    $(document).ready(function () {
        $('.modal').modal();
        $('.dropdown-button').dropdown({
                constrainWidth: false,
            }
        );
    });
}

ymaps.ready(() => {
    initPicker();

    map = new ymaps.Map("map", {
        center: [55.76, 37.64],
        zoom: 7
    });

    $.getJSON("/api/session").done((data) => {
        $("#nav_email").text(data.email);
        let number = 0;
        data.in_friends.forEach((el) => {
            $("#dropdown2").append('<li><a>' + el + '</a></li>')
        });
        data.out_friends.forEach((el) => {
            $("#dropdown1").append('<li id="friend' + number + '"><a><i class="material-icons" onclick="deleteFriend(' + number++ + ')">delete</i><poop>' + el + '</poop></a></li>')
        });

        const areas = new ymaps.GeoObjectCollection();
        data.areas.forEach((el) => {
            areas.add(new ymaps.Circle([[el.latitude, el.longitude], el.radius], {}, { fillColor: "00FF0077" }));
        });
        map.geoObjects.add(areas);
    }).fail(() => {
        window.location.replace('login');
    });

    getTrack();
});

function login(signup = false) {
    const email = $("#email").val();
    const password = $("#password").val();

    $.ajax({
        url: signup ? '/api/signup' : '/api/session',
        type: 'post',
        dataType: 'text',
        xhrFields: {
            withCredentials: true
        },

        success: () => {
            window.location.replace('/');
        },
        error: () => {
            alert('Error :(');
        },
        data: JSON.stringify({
            email: email,
            password: password
        })
    });
}

function logout() {
    $.ajax({
        url: '/api/session',
        type: 'delete',
        xhrFields: {
            withCredentials: true
        },
        success: () => {
            window.location.replace('login');
        },
        error: () => {
            window.location.replace('login');
        },
    });
}

function addFriend() {
    let email = $('#friend_email').val();

    $.ajax({
        url: '/api/friends',
        type: 'post',
        dataType: 'text',
        xhrFields: {
            withCredentials: true
        },

        success: () => {
            alert('Friend added');
        },
        error: () => {
            alert('Error :(');
        },
        data: JSON.stringify({
            email: email
        })
    });
}

function deleteFriend(number) {
    let element = $("#friend" + number);
    let email = element.find("poop").text();

    $.ajax({
        url: '/api/friends',
        type: 'delete',
        dataType: 'text',
        xhrFields: {
            withCredentials: true
        },

        success: () => {
            alert('Friend deleted');
            element.remove()
        },
        error: () => {
            alert('Error :(');
        },
        data: JSON.stringify({
            email: email
        })
    });
}