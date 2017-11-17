let currentTrackDate;
let newTrackDate;
let map;

function getTrack() {
    const time = Math.floor(currentTrackDate.getTime() / 1000);
    $.getJSON("/api/track?time=" + time).done((data) => {
        const myCollection = new ymaps.GeoObjectCollection();
        const points = [];
        data.forEach((item) => {
            const point = [item.latitude, item.longitude];
            myCollection.add(new ymaps.Placemark(point, {
                hintContent: item.latitude + ", " + item.longitude,
                balloonContent: 'Время: ' + item.time
            }));
            myCollection.add(new ymaps.GeoObject({
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
            map.geoObjects.add(myCollection);
            map.geoObjects.add(polyline);
            map.setBounds(polyline.geometry.getBounds());
        }
        debugger
    }).fail(() => {
        Materialize.toast('Произошла ошибка при загрузке данных', 10000, 'rounded');
    });
}

ymaps.ready(() => {
    map = new ymaps.Map("map", {
        center: [55.76, 37.64],
        zoom: 7
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

function session() {
    $.getJSON("/api/session").done((data) => {
        $("#nav_email").text(data.email);
    }).fail(() => {
        window.location.replace('login');
    });

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

    picker.pickadate('picker').set('select', new Date());
}