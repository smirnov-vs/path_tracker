let map;
let currentTrackDate;
let newTrackDate;
let currentTrackUser = 'me';

function getTrack() {
    const time = Math.floor(currentTrackDate / 1000);
    $.getJSON("/api/track?time=" + time + '&who=' + currentTrackUser).done((data) => {
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
        selectMonths: true,
        selectYears: 15,
        today: 'Today',
        clear: 'Clear',
        close: 'Ok',
        closeOnSelect: false,
        defaultDate: new Date(),
        onSet: (data) => {
            newTrackDate = data.select;
        },
        onClose: () => {
            if (currentTrackDate !== newTrackDate) {
                currentTrackDate = newTrackDate + 10800000;
                map.geoObjects.removeAll();
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
        data.out_friends.forEach((el) => {
            $("#dropdown1").append('<li id="friend' + number + '"><a><i class="material-icons" onclick="deleteFriend(' + number++ + ')">delete</i><poop>' + el + '</poop></a></li>')
        });
        data.in_friends.forEach((el) => {
            $("#dropdown2").append('<li><a onclick="changeTrackUser(\'' + el + '\')">' + el + '</a></li>')
        });

        const areas = new ymaps.GeoObjectCollection();
        data.areas.forEach((el) => {
            $("#dropdown3").append('<li data-id="' + el.id + '"><a>' + el.name + '</a></li>')
            const coords = [];
            el.coordinates.forEach((c) => {
                coords.push([c.latitude, c.longitude]);
            });
            areas.add(new ymaps.Polygon([coords], {hintContent: el.name}, {fillColor: "#00FF0077",}));
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

function addArea() {
    const area = new ymaps.Polygon([], {}, {
        editorDrawingCursor: "crosshair",
        fillColor: '#00FF0077',
        strokeColor: '#0000FF',
        strokeWidth: 5,
    });
    map.geoObjects.add(area);

    map.geoObjects.events.add('editorstatechange', (e) => {
        const state = area.editor.state;
        if (!state.get('drawing') && state.get('editing')) {
            const coords = area.geometry._coordPath.getCoordinates()[0];
            console.log(coords.length);
            if (coords.length < 4) {
                setTimeout(() => { map.geoObjects.remove(area) }, 1);
                return;
            }

            let name = "";
            while (name === "") {
                name = prompt("Введите название зоны");
            }
            if (name != null) {
                sendArea(name, coords, area);
            } else {
                setTimeout(() => { map.geoObjects.remove(area) }, 1);
            }
        }
    });

    area.editor.startDrawing();
}

function sendArea(name, coords, polygon) {
    let coordinates = [];
    coords.forEach((c) => {
        coordinates.push({
            "latitude": c[0],
            "longitude": c[1]
        })
    });

    $.ajax({
        url: '/api/areas',
        type: 'post',
        dataType: 'text',
        xhrFields: {
            withCredentials: true
        },

        success: () => {
            map.geoObjects.remove(polygon);
            map.geoObjects.add(polygon);
        },
        error: () => {
            map.geoObjects.remove(polygon);
            alert('Error :(');
        },
        data: JSON.stringify({
            name: name,
            coordinates: coordinates
        })
    });
}

function changeTrackUser(user) {
    currentTrackUser = user;
    map.geoObjects.removeAll();
    getTrack();
}