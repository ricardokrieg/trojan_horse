window.check_group_actions_shadow = function() {
    if ($('#group-actions').position().top < ($('#group').position().top + $('#group').height() - $(document).scrollTop())) {
        $('#group-actions').addClass('over');
    } else {
        $('#group-actions').removeClass('over');
    }
}

window.update_client = function() {
    last_time = $.now();
    update = true;

    setInterval(function(){
        if (update) {
            update = false;

            $.get('/clients/'+window.client_id+'/image', function(data){
                $('#image').attr('src', data);

                $('#timer span').html((($.now()-last_time)/1000).toFixed(3) + 's');
                last_time = $.now();

                update = true;
            });
        }
    }, 50);
}

window.remove_name = function() {
    $('#client-name').val('');
    $('#form-client').submit();
}
