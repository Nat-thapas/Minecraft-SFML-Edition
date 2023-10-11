uniform sampler2D texture;
uniform float time;

void main() {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    gl_FragColor = vec4(gl_Color.rgb * pixel.rgb * (step(0.0, pixel.aaa)) * (time * pixel.aaa + (1.0 - time)),
                        gl_Color.a * pixel.a + min((1.0 - pixel.a) * (1.0 - gl_Color.r), time));
}
