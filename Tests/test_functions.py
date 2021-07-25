import sys
import pyjion
import pyjion.dis
import unittest
import math
import time
import pytest


class ScopeLeaksTestCase:
    def test_slice(self):
        a = "12345"

        def x(a):
            a = a[1:]
        before = sys.getrefcount(a)
        x(a)
        assert before == sys.getrefcount(a)

    def test_inplace_operator(self):
        a = "12345"

        def x(a):
            a += a
        before = sys.getrefcount(a)
        x(a)
        assert before == sys.getrefcount(a)


class FunctionCallsTestCase:

    def test_arg0(self):
        def arg0() -> int:
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d

        assert sys.getrefcount(arg0) == 2
        assert arg0() == 10
        assert sys.getrefcount(arg0) == 2
        info = pyjion.info(arg0)
        assert info['compiled'], info['compile_result']

    def test_arg0_cfunction(self):
        target = time.time
        pre_ref_cnt = sys.getrefcount(target)
        assert target() is not None
        assert sys.getrefcount(target) == pre_ref_cnt
        info = pyjion.info(self.test_arg0_cfunction.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg0_exc(self):
        def arg0() -> int:
            raise ValueError

        assert sys.getrefcount(arg0) == 2
        with pytest.raises(ValueError):
            arg0()
        assert sys.getrefcount(arg0) == 2
        info = pyjion.info(arg0)
        assert info['compiled'], info['compile_result']

    def test_arg0_cfunction_exc(self):
        target = math.sqrt
        pre_ref_cnt = sys.getrefcount(target)
        with pytest.raises(TypeError):
            target()
        assert sys.getrefcount(target) == pre_ref_cnt
        info = pyjion.info(self.test_arg0_cfunction_exc.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg1(self):
        def arg1(e):
            a = '1'
            b = '2'
            c = '3'
            d = '4'
            return a + b + c + d + e

        a = '5'
        pre_ref = sys.getrefcount(a)
        assert sys.getrefcount(arg1) == 2
        assert arg1(a) == '12345'
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(a) == pre_ref

        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg1_cfunction(self):
        target = math.sqrt
        four = 1024
        pre_ref_cnt = sys.getrefcount(target)
        arg1_pre_ref_cnt = sys.getrefcount(four)
        assert target(four) == 32.0
        assert sys.getrefcount(target) == pre_ref_cnt
        assert sys.getrefcount(four) == arg1_pre_ref_cnt
        info = pyjion.info(self.test_arg1_cfunction.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg1_exc(self):
        def arg1(e):
            raise ValueError

        a = '5'
        pre_ref = sys.getrefcount(a)
        assert sys.getrefcount(arg1) == 2
        with pytest.raises(ValueError):
            arg1(a)
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(a) == pre_ref

        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg1_cfunction_exc(self):
        target = math.sqrt
        four = 'four'
        pre_ref_cnt = sys.getrefcount(target)
        arg1_pre_ref_cnt = sys.getrefcount(four)
        with pytest.raises(TypeError):
            target((four,))
        assert sys.getrefcount(target) == pre_ref_cnt
        assert sys.getrefcount(four) == arg1_pre_ref_cnt
        info = pyjion.info(self.test_arg1_cfunction_exc.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg2(self):
        def arg2(e, f):
            a = '1'
            b = '2'
            c = '3'
            d = '4'
            return a + b + c + d + e + f

        a = '5'
        b = '6'
        pre_ref_a = sys.getrefcount(a)
        pre_ref_b = sys.getrefcount(b)
        assert sys.getrefcount(arg2) == 2
        assert arg2(a, b) == '123456'
        assert sys.getrefcount(arg2) == 2
        assert sys.getrefcount(a) == pre_ref_a
        assert sys.getrefcount(b) == pre_ref_b

        info = pyjion.info(arg2)
        assert info['compiled'], info['compile_result']

    def test_arg3(self):
        def arg3(e, f, g):
            a = '1'
            b = '2'
            c = '3'
            d = '4'
            return a + b + c + d + e + f + g

        assert arg3('5', '6', '7') == '1234567'
        info = pyjion.info(arg3)
        assert info['compiled'], info['compile_result']

    def test_arg4(self):
        def arg4(e, f, g, h):
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d + e + f + g + h

        assert arg4(5, 6, 7, 8) == 36
        info = pyjion.info(arg4)
        assert info['compiled'], info['compile_result']

    def test_arg5(self):
        def arg5(e, f, g, h, i):
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d + e + f + g + h + i

        assert arg5(5, 6, 7, 8, 9) == 45
        info = pyjion.info(arg5)
        assert info['compiled'], info['compile_result']

    def test_arg6(self):
        def arg6(e, f, g, h, i, j):
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d + e + f + g + h + i + j

        assert arg6(5, 6, 7, 8, 9, 10) == 55
        info = pyjion.info(arg6)
        assert info['compiled'], info['compile_result']

    def test_arg7(self):
        def arg7(e, f, g, h, i, j, k):
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d + e + f + g + h + i + j + k

        assert arg7(5, 6, 7, 8, 9, 10, 11) == 66
        info = pyjion.info(arg7)
        assert info['compiled'], info['compile_result']

    def test_arg8(self):
        def arg8(e, f, g, h, i, j, k, l):
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d + e + f + g + h + i + j + k + l

        assert arg8(5, 6, 7, 8, 9, 10, 11, 12) == 78
        info = pyjion.info(arg8)
        assert info['compiled'], info['compile_result']

    def test_arg9(self):
        def arg9(e, f, g, h, i, j, k, l, m):
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d + e + f + g + h + i + j + k + l + m

        assert arg9(5, 6, 7, 8, 9, 10, 11, 12, 13) == 91
        info = pyjion.info(arg9)
        assert info['compiled'], info['compile_result']

    def test_arg10(self):
        def arg10(e, f, g, h, i, j, k, l, m, n):
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d + e + f + g + h + i + j + k + l + m + n

        assert arg10(5, 6, 7, 8, 9, 10, 11, 12, 13, 14) == 105
        info = pyjion.info(arg10)
        assert info['compiled'], info['compile_result']

    def test_arg11(self):
        def arg11(e, f, g, h, i, j, k, l, m, n, o):
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d + e + f + g + h + i + j + k + l + m + n + o

        assert arg11(5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15) == 120
        info = pyjion.info(arg11)
        assert info['compiled'], info['compile_result']

    def test_arg15(self):
        def arg15(e, f, g, h, i, j, k, l, m, n, o, p, q, r, s):
            a = '1'
            b = '2'
            c = '3'
            d = '4'
            return a + b + c + d + e + f + g + h + i + j + k + l + m + n + o

        a = '5'
        b = '6'
        pre_ref_a = sys.getrefcount(a)
        pre_ref_b = sys.getrefcount(b)
        assert sys.getrefcount(arg15) == 2
        assert arg15(a, b, '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19') == '123456789101112131415'
        assert sys.getrefcount(arg15) == 2
        assert sys.getrefcount(a) == pre_ref_a
        assert sys.getrefcount(b) == pre_ref_b
        info = pyjion.info(arg15)
        assert info['compiled'], info['compile_result']

    def test_arg15_cfunction(self):
        a = 500
        b = 600
        target = math.hypot
        pre_ref_target = sys.getrefcount(target)
        pre_ref_a = sys.getrefcount(a)
        pre_ref_b = sys.getrefcount(b)
        assert target(a, b, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) == 782.54648424231
        assert sys.getrefcount(target) == pre_ref_target
        assert sys.getrefcount(a) == pre_ref_a
        assert sys.getrefcount(b) == pre_ref_b
        info = pyjion.info(self.test_arg15_cfunction.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg15_exc(self):
        def arg15(e, f, g, h, i, j, k, l, m, n, o, p, q, r, s):
            raise ValueError

        a = '5'
        b = '6'
        pre_ref_a = sys.getrefcount(a)
        pre_ref_b = sys.getrefcount(b)
        assert sys.getrefcount(arg15) == 2
        with pytest.raises(ValueError):
            arg15(a, b, '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19')
        assert sys.getrefcount(arg15) == 2
        assert sys.getrefcount(a) == pre_ref_a
        assert sys.getrefcount(b) == pre_ref_b
        info = pyjion.info(arg15)
        assert info['compiled'], info['compile_result']


class ClassMethodCallsTestCase:

    def test_arg0(self):
        class F:
            @classmethod
            def arg0(cls) -> int:
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d

        assert sys.getrefcount(F.arg0) == 1
        assert F.arg0() == 10
        assert sys.getrefcount(F.arg0) == 1
        info = pyjion.info(F.arg0.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg0_exc(self):
        class F:
            @classmethod
            def arg0(cls) -> int:
                raise ValueError

        assert sys.getrefcount(F.arg0) == 1
        with pytest.raises(ValueError):
            F.arg0()
        assert sys.getrefcount(F.arg0) == 1
        info = pyjion.info(F.arg0.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg1(self):
        class F:
            @classmethod
            def arg1(cls, e):
                a = '1'
                b = '2'
                c = '3'
                d = '4'
                return a + b + c + d + e

        arg_a = 'e'
        pre_ref_cnt = sys.getrefcount(arg_a)
        assert sys.getrefcount(F) == 5
        assert sys.getrefcount(F.arg1) == 1
        assert F.arg1(arg_a) == '1234e'
        assert sys.getrefcount(arg_a) == pre_ref_cnt
        assert sys.getrefcount(F) == 5
        assert sys.getrefcount(F.arg1) == 1

        info = pyjion.info(F.arg1.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg1_cfunction(self):
        arg_a = 'jeremy'
        target = str.title
        arg_a_pre_ref = sys.getrefcount(arg_a)
        target_pre_ref = sys.getrefcount(target)
        target(arg_a)
        assert sys.getrefcount(target) == target_pre_ref
        assert sys.getrefcount(arg_a) == arg_a_pre_ref
        info = pyjion.info(self.test_arg1_cfunction.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg1_cfunction_exc(self):
        arg_a = 50000
        target = str.title
        arg_a_pre_ref = sys.getrefcount(arg_a)
        target_pre_ref = sys.getrefcount(target)
        with pytest.raises(TypeError):
            target((arg_a,))
        assert sys.getrefcount(target) == target_pre_ref
        assert sys.getrefcount(arg_a) == arg_a_pre_ref
        info = pyjion.info(self.test_arg1_cfunction_exc.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg1_exc(self):
        class F:
            @classmethod
            def arg1(cls, e):
                raise ValueError

        arg_a = 'e'
        pre_ref_cnt = sys.getrefcount(arg_a)
        assert sys.getrefcount(F) == 5
        assert sys.getrefcount(F.arg1) == 1
        with pytest.raises(ValueError):
            F.arg1(arg_a)
        assert sys.getrefcount(arg_a) == pre_ref_cnt
        assert sys.getrefcount(F) == 5
        assert sys.getrefcount(F.arg1) == 1

        info = pyjion.info(F.arg1.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg2(self):
        class F:
            @classmethod
            def arg2(cls, e, f):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f

        assert F.arg2(5, 6) == 21
        info = pyjion.info(F.arg2.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg3(self):
        class F:
            @classmethod
            def arg3(cls, e, f, g):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g

        assert F.arg3(5, 6, 7) == 28
        info = pyjion.info(F.arg3.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg4(self):
        class F:
            @classmethod
            def arg4(cls, e, f, g, h):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h

        assert F.arg4(5, 6, 7, 8) == 36
        info = pyjion.info(F.arg4.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg5(self):
        class F:
            @classmethod
            def arg5(cls, e, f, g, h, i):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i

        assert F.arg5(5, 6, 7, 8, 9) == 45
        info = pyjion.info(F.arg5.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg6(self):
        class F:
            @classmethod
            def arg6(cls, e, f, g, h, i, j):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j

        assert F.arg6(5, 6, 7, 8, 9, 10) == 55
        info = pyjion.info(F.arg6.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg7(self):
        class F:
            @classmethod
            def arg7(cls, e, f, g, h, i, j, k):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k

        assert F.arg7(5, 6, 7, 8, 9, 10, 11) == 66
        info = pyjion.info(F.arg7.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg8(self):
        class F:
            @classmethod
            def arg8(cls, e, f, g, h, i, j, k, l):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k + l

        assert F.arg8(5, 6, 7, 8, 9, 10, 11, 12) == 78
        info = pyjion.info(F.arg8.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg9(self):
        class F:
            @classmethod
            def arg9(cls, e, f, g, h, i, j, k, l, m):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k + l + m

        assert F.arg9(5, 6, 7, 8, 9, 10, 11, 12, 13) == 91
        info = pyjion.info(F.arg9.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg10(self):
        class F:
            @classmethod
            def arg10(cls, e, f, g, h, i, j, k, l, m, n):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k + l + m + n

        assert F.arg10(5, 6, 7, 8, 9, 10, 11, 12, 13, 14) == 105
        info = pyjion.info(F.arg10.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg11(self):
        class F:
            @classmethod
            def arg11(cls, e, f, g, h, i, j, k, l, m, n, o):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k + l + m + n + o

        assert F.arg11(5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15) == 120
        info = pyjion.info(F.arg11.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg15(self):
        class F:
            @classmethod
            def arg15(cls, e, f, g, h, i, j, k, l, m, n, o, p ,q ,r,s):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p + q + r + s

        a = 10000
        pre_ref_cnt = sys.getrefcount(a)
        pre_target_cnt = sys.getrefcount(F.arg15)
        assert F.arg15(a, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) == 10185
        assert sys.getrefcount(a) == pre_ref_cnt
        assert sys.getrefcount(F.arg15) == pre_target_cnt
        info = pyjion.info(F.arg15.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg15_cfunction_exc(self):
        target = str.strip
        a = '  aa  '
        pre_ref_cnt = sys.getrefcount(a)
        pre_target_cnt = sys.getrefcount(target)
        with pytest.raises(TypeError):
            target((a, '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19'))
        assert sys.getrefcount(a) == pre_ref_cnt
        assert sys.getrefcount(target) == pre_target_cnt
        info = pyjion.info(self.test_arg15_cfunction_exc.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg15_exc(self):
        class F:
            @classmethod
            def arg15(cls, e, f, g, h, i, j, k, l, m, n, o, p ,q ,r,s):
                raise ValueError

        a = '1'
        pre_ref_cnt = sys.getrefcount(a)
        pre_target_cnt = sys.getrefcount(F.arg15)
        with pytest.raises(ValueError):
            F.arg15(a, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19)
        assert sys.getrefcount(a) == pre_ref_cnt
        assert sys.getrefcount(F.arg15) == pre_target_cnt
        info = pyjion.info(F.arg15.__code__)
        assert info['compiled'], info['compile_result']


class FunctionKwCallsTestCase:

    def test_arg1(self):
        def arg1(e):
            a = '1'
            b = '2'
            c = '3'
            d = '4'
            return a + b + c + d + e

        a = '5'
        pre_ref = sys.getrefcount(a)
        assert sys.getrefcount(arg1) == 2
        assert arg1(e=a) == '12345'
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(a) == pre_ref

        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg1_unpack_tuple(self):
        def arg1(e):
            a = '1'
            b = '2'
            c = '3'
            d = '4'
            return a + b + c + d + e

        args = ('5',)
        pre_ref = sys.getrefcount(args)
        assert sys.getrefcount(arg1) == 2
        assert arg1(*args) == '12345'
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(args) == pre_ref

        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg1_unpack_tuple_exc(self):
        def arg1(e):
            raise ValueError

        args = ('5',)
        pre_ref = sys.getrefcount(args)
        assert sys.getrefcount(arg1) == 2
        with pytest.raises(ValueError):
            arg1(*args)
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(args) == pre_ref
        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg1_unpack_dict(self):
        def arg1(e):
            a = '1'
            b = '2'
            c = '3'
            d = '4'
            return a + b + c + d + e

        args = {'e': '5'}
        pre_ref = sys.getrefcount(args)
        assert sys.getrefcount(arg1) == 2
        assert arg1(**args) == '12345'
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(args) == pre_ref

        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg1_unpack_dict_exc(self):
        def arg1(e):
            raise ValueError

        args = {'e': '5'}
        pre_ref = sys.getrefcount(args)
        assert sys.getrefcount(arg1) == 2
        with pytest.raises(ValueError):
            arg1(**args)
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(args) == pre_ref

        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg1_unpack_dict_and_tuple(self):
        def arg1(e, f):
            a = '1'
            b = '2'
            c = '3'
            d = '4'
            return a + b + c + d + e + f

        args = ('5',)
        kargs = {'f': '6'}
        pre_ref = sys.getrefcount(args)
        kpre_ref = sys.getrefcount(kargs)
        assert sys.getrefcount(arg1) == 2
        assert arg1(*args, **kargs) == '123456'
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(args) == pre_ref
        assert sys.getrefcount(kargs) == kpre_ref

        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg1_unpack_dict_and_tuple_exc(self):
        def arg1(e, f):
            raise ValueError

        args = ('5',)
        kargs = {'f': '6'}
        pre_ref = sys.getrefcount(args)
        kpre_ref = sys.getrefcount(kargs)
        assert sys.getrefcount(arg1) == 2
        with pytest.raises(ValueError):
            arg1(*args, **kargs)
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(args) == pre_ref
        assert sys.getrefcount(kargs) == kpre_ref

        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg1_exc(self):
        def arg1(e):
            raise ValueError

        a = '5'
        pre_ref = sys.getrefcount(a)
        assert sys.getrefcount(arg1) == 2
        with pytest.raises(ValueError):
            arg1(e=a)
        assert sys.getrefcount(arg1) == 2
        assert sys.getrefcount(a) == pre_ref

        info = pyjion.info(arg1)
        assert info['compiled'], info['compile_result']

    def test_arg3(self):
        def arg3(e, f=None, *args, **kwargs):
            a = '1'
            b = '2'
            c = '3'
            d = '4'
            return a + b + c + d + e + f

        a = '5'
        b = '6'
        c = '7'
        pre_ref_a = sys.getrefcount(a)
        pre_ref_b = sys.getrefcount(b)
        pre_ref_c = sys.getrefcount(c)
        assert sys.getrefcount(arg3) == 2
        assert arg3(a, f=b, g=c) == '123456'

        assert sys.getrefcount(arg3) == 2
        assert sys.getrefcount(a) == pre_ref_a
        assert sys.getrefcount(b) == pre_ref_b
        assert sys.getrefcount(c) == pre_ref_c

        info = pyjion.info(arg3)
        assert info['compiled'], info['compile_result']

    def test_arg3_exc(self):
        def arg3(e, f=None, *args, **kwargs):
            raise ValueError

        a = '5'
        b = '6'
        c = '7'
        pre_ref_a = sys.getrefcount(a)
        pre_ref_b = sys.getrefcount(b)
        pre_ref_c = sys.getrefcount(c)
        assert sys.getrefcount(arg3) == 2
        with pytest.raises(ValueError):
            arg3(a, f=b, g=c)

        assert sys.getrefcount(arg3) == 2
        assert sys.getrefcount(a) == pre_ref_a
        assert sys.getrefcount(b) == pre_ref_b
        assert sys.getrefcount(c) == pre_ref_c

        info = pyjion.info(arg3)
        assert info['compiled'], info['compile_result']


class ObjectMethodCallsTestCase:

    def test_arg0(self):
        class F:
            def arg0(cls) -> int:
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d

        f = F()
        assert sys.getrefcount(F.arg0) == 2
        assert sys.getrefcount(F) == 6
        assert sys.getrefcount(f) == 2
        assert f.arg0() == 10
        assert sys.getrefcount(F.arg0) == 2
        assert sys.getrefcount(F) in [6, 7]
        assert sys.getrefcount(f) == 2
        info = pyjion.info(f.arg0.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg0_cfunction(self):
        f = str("hello")
        pre_target_ref = sys.getrefcount(str.title)
        pre_arg_ref = sys.getrefcount(f)
        assert f.title() == "Hello"
        assert sys.getrefcount(f) == pre_arg_ref
        assert sys.getrefcount(str.title) == pre_target_ref
        info = pyjion.info(self.test_arg0_cfunction.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg1(self):
        class F:
            def arg1(self, e):
                a = '1'
                b = '2'
                c = '3'
                d = '4'
                return a + b + c + d + e

        f = F()
        test_arg1_arg1 = '5'
        pre_refcnt_a = sys.getrefcount(test_arg1_arg1)
        assert sys.getrefcount(F.arg1) == 2
        assert sys.getrefcount(F) == 6
        assert sys.getrefcount(f) == 2
        assert f.arg1(test_arg1_arg1) == '12345'
        assert sys.getrefcount(F.arg1) == 2
        assert sys.getrefcount(F) in [6, 7]
        assert sys.getrefcount(f) == 2
        assert pre_refcnt_a == sys.getrefcount(test_arg1_arg1)
        info = pyjion.info(f.arg1.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg1_cfunction(self):
        f = str("hello")
        o = "o"
        pre_target_ref = sys.getrefcount(str.strip)
        pre_arg_ref = sys.getrefcount(f)
        pre_arg1_ref = sys.getrefcount(o)
        assert f.strip(o) == "hell"
        assert sys.getrefcount(f) == pre_arg_ref
        assert sys.getrefcount(o) == pre_arg1_ref
        assert sys.getrefcount(str.strip) == pre_target_ref
        info = pyjion.info(self.test_arg1_cfunction.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg1_cfunction_exc(self):
        f = str("hello")
        o = 1000000
        pre_target_ref = sys.getrefcount(str.strip)
        pre_arg_ref = sys.getrefcount(f)
        pre_arg1_ref = sys.getrefcount(o)
        with pytest.raises(TypeError):
            f.strip((o,))
        assert sys.getrefcount(f) == pre_arg_ref
        assert sys.getrefcount(o) == pre_arg1_ref
        assert sys.getrefcount(str.strip) == pre_target_ref
        info = pyjion.info(self.test_arg1_cfunction_exc.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg2(self):
        class F:
            def arg2(self, e, f):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f

        f = F()
        assert f.arg2(5, 6) == 21
        info = pyjion.info(f.arg2.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg3(self):
        class F:
            def arg3(self, e, f, g):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g

        f = F()
        assert f.arg3(5, 6, 7) == 28
        info = pyjion.info(f.arg3.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg4(self):
        class F:
            def arg4(self, e, f, g, h):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h

        f = F()
        assert f.arg4(5, 6, 7, 8) == 36
        info = pyjion.info(f.arg4.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg5(self):
        class F:
            def arg5(self, e, f, g, h, i):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i

        f = F()
        assert f.arg5(5, 6, 7, 8, 9) == 45
        info = pyjion.info(f.arg5.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg6(self):
        class F:
            def arg6(self, e, f, g, h, i, j):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j

        f = F()
        assert f.arg6(5, 6, 7, 8, 9, 10) == 55
        info = pyjion.info(f.arg6.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg7(self):
        class F:
            def arg7(self, e, f, g, h, i, j, k):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k

        f = F()
        assert f.arg7(5, 6, 7, 8, 9, 10, 11) == 66
        info = pyjion.info(f.arg7.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg8(self):
        class F:
            def arg8(self, e, f, g, h, i, j, k, l):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k + l

        f = F()
        assert f.arg8(5, 6, 7, 8, 9, 10, 11, 12) == 78
        info = pyjion.info(f.arg8.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg9(self):
        class F:
            def arg9(self, e, f, g, h, i, j, k, l, m):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k + l + m

        f = F()
        assert f.arg9(5, 6, 7, 8, 9, 10, 11, 12, 13) == 91
        info = pyjion.info(f.arg9.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg10(self):
        class F:
            def arg10(self, e, f, g, h, i, j, k, l, m, n):
                a = 1
                b = 2
                c = 3
                d = 4
                return a + b + c + d + e + f + g + h + i + j + k + l + m + n

        f = F()
        assert f.arg10(5, 6, 7, 8, 9, 10, 11, 12, 13, 14) == 105
        info = pyjion.info(f.arg10.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg15(self):
        class F:
            def arg15(self, e, f, g, h, i, j, k, l, m, n, o, p, q, r):
                a = '1'
                b = '2'
                c = '3'
                d = '4'
                return a + b + c + d + e + f + g + h + i + j + k + l + m + n + o

        f = F()
        arg1 = '5'
        pre_refcnt_a = sys.getrefcount(arg1)
        assert sys.getrefcount(F.arg15) == 2
        assert sys.getrefcount(F) == 6
        assert sys.getrefcount(f) == 2
        assert f.arg15(arg1, '6', '7', '8', '9', '10', '11', '12', '13', '14', '15' ,'16', '17', '18') == '123456789101112131415'
        assert sys.getrefcount(F.arg15) == 2
        assert sys.getrefcount(F) in [6, 7]
        assert sys.getrefcount(f) == 2
        assert pre_refcnt_a == sys.getrefcount(arg1)
        info = pyjion.info(f.arg15.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg15_exc(self):
        class F:
            def arg15(self, e, f, g, h, i, j, k, l, m, n, o, p, q, r):
                raise ValueError
        f = F()
        arg1 = '5'
        pre_refcnt_a = sys.getrefcount(arg1)
        assert sys.getrefcount(F.arg15) == 2
        assert sys.getrefcount(F) == 6
        assert sys.getrefcount(f) == 2
        with pytest.raises(ValueError):
            f.arg15(arg1, '6', '7', '8', '9', '10', '11', '12', '13', '14', '15' ,'16', '17', '18')
        assert sys.getrefcount(F.arg15) == 2
        assert sys.getrefcount(F) == 6
        assert sys.getrefcount(f) == 2
        assert pre_refcnt_a == sys.getrefcount(arg1)
        info = pyjion.info(f.arg15.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg15_cfunction(self):
        f = str("{}{}{}{}{}{}{}{}{}{}{}{}{}{}")
        arg1 = '5'
        pre_refcnt = sys.getrefcount(f)
        pre_refcnt_a = sys.getrefcount(arg1)
        target_pre_refcnt = sys.getrefcount(str.format)
        assert f.format(arg1, '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18') == "56789101112131415161718"
        assert sys.getrefcount(f) == pre_refcnt
        assert sys.getrefcount(str.format) == target_pre_refcnt
        assert sys.getrefcount(arg1) == pre_refcnt_a
        info = pyjion.info(self.test_arg15_cfunction.__code__)
        assert info['compiled'], info['compile_result']

    def test_arg15_cfunction_exc(self):
        f = str("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}")
        arg1 = '5'
        pre_refcnt = sys.getrefcount(f)
        pre_refcnt_a = sys.getrefcount(arg1)
        target_pre_refcnt = sys.getrefcount(str.format)
        with pytest.raises(IndexError):
            f.format((arg1, '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18'))
        assert sys.getrefcount(f) == pre_refcnt
        assert sys.getrefcount(str.format) == target_pre_refcnt
        assert sys.getrefcount(arg1) == pre_refcnt_a
        info = pyjion.info(self.test_arg15_cfunction_exc.__code__)
        assert info['compiled'], info['compile_result']


if __name__ == "__main__":
    unittest.main()
